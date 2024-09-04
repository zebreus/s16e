#pragma once

#include <array>
#include <lwip/netdb.h>
#include <string_view>

// A provides a interface to manage a client connection and clean up using RAII
class SocketWrapper {
private:
  int socket;
  sockaddr source_addr;
  std::array<char, 128> address_string;

public:
  // Create a new client connection for a given socket file descriptor and
  // source address
  SocketWrapper(int socket, sockaddr source);

  // Move construction that copies other into this and leaves other in an
  // undefined state
  SocketWrapper(SocketWrapper &&other);

  // Free connection
  ~SocketWrapper();

  // Disable copies and assignments, as they could create two connections with
  // the same socket or overwrite a connection without closing it.
  SocketWrapper(const SocketWrapper &other) = delete;
  SocketWrapper &operator=(const SocketWrapper &other) = delete;
  SocketWrapper &operator=(SocketWrapper &&other) = delete;

  int getSocket();
  const char *getAddress();
};
