#include "SocketWrapper.hpp"
#include "../config.hpp"
#include "esp_log.h"
#include "lwip/sockets.h"

SocketWrapper::SocketWrapper(int socket, sockaddr source)
    : socket(socket), source_addr(source) {

  // Set tcp keepalive option
  int one = 1;
  setsockopt(this->socket, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(int));
  setsockopt(this->socket, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(int));
  setsockopt(this->socket, IPPROTO_TCP, TCP_KEEPIDLE, &KEEPALIVE_IDLE,
             sizeof(int));
  setsockopt(this->socket, IPPROTO_TCP, TCP_KEEPINTVL, &KEEPALIVE_INTERVAL,
             sizeof(int));
  setsockopt(this->socket, IPPROTO_TCP, TCP_KEEPCNT, &KEEPALIVE_COUNT,
             sizeof(int));

  // Set nonblocking option
  fcntl(this->socket, F_SETFL, fcntl(this->socket, F_GETFL, 0) | O_NONBLOCK);

  // Convert ip address to string
  inet_ntoa_r(((struct sockaddr_in *)&this->source_addr)->sin_addr,
              this->address_string.data(), this->address_string.size() - 1);
};

// Move construction that copies other into this and leaves other in an
// undefined state
SocketWrapper::SocketWrapper(SocketWrapper &&other)
    : socket(other.socket), source_addr(other.source_addr),
      address_string(other.address_string) {
  other.socket = -1;
};

SocketWrapper::~SocketWrapper() {
  if (this->socket == -1) {
    return;
  }
  ESP_LOGE(NETWORKING_LOG_TAG, "Closing connections to %i", this->socket);
  shutdown(this->socket, 0);
  close(this->socket);
}

int SocketWrapper::getSocket() { return socket; };

const char *SocketWrapper::getAddress() { return address_string.data(); }