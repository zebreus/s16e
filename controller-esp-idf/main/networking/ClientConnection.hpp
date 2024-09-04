#pragma once

#include "../RingBuffer.hpp"
#include "../config.hpp"
#include "SocketWrapper.hpp"
#include <array>
#include <lwip/netdb.h>
#include <optional>
#include <span>

// A provides a interface to manage a client connection and clean up using RAII
class ClientConnection {
private:
  std::optional<SocketWrapper> socket = std::nullopt;

  std::array<char, RECEIVE_BYTES_PER_FRAME> inputBuffer;
  size_t inputBufferLength = 0;

public:
  // Accept an incoming connection on the serversocket.
  // Does nothing if there is no connection available on the serversocket
  // If there is already a connection
  void acceptConnection(int serversocket);

  // Disable copies and assignments, as they could create two connections with
  // the same socket or overwrite a connection without closing it.
  ClientConnection() = default;
  ClientConnection(const ClientConnection &other) = delete;
  ClientConnection &operator=(const ClientConnection &other) = delete;
  ClientConnection &operator=(ClientConnection &&other) = delete;

  // Check if this client is currently connected
  // Does not refresh the network state,
  bool active();

  // Read the data for the next frame into the into the input buffer
  void receiveFrame();
  // Send the data for the current frame
  void sendFrame();

  // Send some data
  int sendBuffer(std::span<char> &buffer);
  // Send some data
  void sendFromRingBuffer(RingBuffer<SEND_QUEUE_SIZE> &buffer);
  // Receive some data. You need to mark the number of bytes that were processed
  // with markBytesAsRead afterwards
  std::span<char> receiveBuffer();
  void markBytesAsRead(int bytes);

  pollfd getPollFd();

private:
  // Close the connection and replace the socket with -1
  // This is error handling
  void killConnection();
};