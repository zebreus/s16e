#include "ClientConnection.hpp"
#include "../config.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"
#include "lwip/sockets.h"
#include <array>
#include <cstring>
#include <lwip/netdb.h>
#include <optional>
#include <sys/param.h>

void ClientConnection::acceptConnection(int server_socket) {
  struct sockaddr source_address;
  socklen_t addr_len = sizeof(source_address);
  auto new_socket = accept(server_socket, &source_address, &addr_len);
  if (new_socket < 0) {
    ESP_LOGE(NETWORKING_LOG_TAG, "Failed to accept connection: errno %d",
             errno);
    return;
  }

  this->socket.emplace(new_socket, source_address);
};

void ClientConnection::killConnection() { this->socket.reset(); }
bool ClientConnection::active() { return this->socket.has_value(); }

// Read the data for the next frame into the into the input buffer
void ClientConnection::receiveFrame() {
  int maxReceiveBytes = RECEIVE_BYTES_PER_FRAME - inputBufferLength;
  if (maxReceiveBytes == 0) {
    return;
  }

  int result = recv(socket->getSocket(), inputBuffer.data() + inputBufferLength,
                    maxReceiveBytes, MSG_DONTWAIT);
  if (result < 0) {
    switch (errno) {
    case EWOULDBLOCK:
    case ENOENT: // caused by vfs
      return;
    case ENOTCONN:
    case EPIPE:
    case ECONNRESET:
    case ECONNREFUSED:
    case ECONNABORTED:
      killConnection();
      return;
    default:
      ESP_LOGE(NETWORKING_LOG_TAG,
               "Something weird happend on connection with %s; result %i",
               socket->getAddress(), errno);
      return;
    }
  }

  this->inputBufferLength += result;
}

pollfd ClientConnection::getPollFd() {
  pollfd result;

  result.fd = socket.transform([](auto &client) { return client.getSocket(); })
                  .value_or(-1);
  result.events = POLLIN | POLLERR;
  result.revents = 0;

  return result;
}

// Send some data
void ClientConnection::sendFromRingBuffer(RingBuffer<SEND_QUEUE_SIZE> &buffer) {
  if (socket->getSocket() == -1) {
    // No connection
    return;
  }
  auto sendBuffers = buffer.read(SEND_BYTES_PER_FRAME);

  if (!sendBuffers.has_value()) {
    return;
  }
  auto &firstBuffer = sendBuffers->first;
  auto &secondBuffer = sendBuffers->second;
  ssize_t firstSentBytes = send(socket->getSocket(), firstBuffer.data(),
                                firstBuffer.size_bytes(), MSG_DONTWAIT);
  if (firstSentBytes < 0) {
    ESP_LOGE(NETWORKING_LOG_TAG,
             "Failed to send some bytes. Killing connection");
    killConnection();
    return;
  }
  buffer.markConsumed(firstSentBytes);

  if (secondBuffer.size_bytes() == 0) {
    return;
  }

  ssize_t secondSendBytes = send(socket->getSocket(), secondBuffer.data(),
                                 secondBuffer.size_bytes(), MSG_DONTWAIT);
  if (secondSendBytes < 0) {
    ESP_LOGE(NETWORKING_LOG_TAG,
             "Failed to send some bytes. Killing connection");
    killConnection();
    return;
  }
  buffer.markConsumed(secondSendBytes);
}

// Receive some data
std::span<char> ClientConnection::receiveBuffer() {
  return std::span<char>(inputBuffer.data(), inputBufferLength);
}

void ClientConnection::markBytesAsRead(int bytes) {
  int remainingBytes = inputBufferLength - bytes;
  if (bytes < 0) {
    bytes = 0;
  }
  int consumedBytes = inputBufferLength - remainingBytes;

  for (int i = 0; i < remainingBytes; i++) {
    inputBuffer[i] = inputBuffer[i + consumedBytes];
  }
  inputBufferLength = remainingBytes;
}
