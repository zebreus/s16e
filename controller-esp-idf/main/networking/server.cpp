#include "../config.hpp"
#include "../state/setup.hpp"
#include "ClientConnection.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"
#include "lwip/sockets.h"
#include <array>
#include <lwip/netdb.h>
#include <string.h>
#include <sys/param.h>

std::array<ClientConnection, MAX_CLIENTS> clients;
std::array<State, MAX_CLIENTS> states;

// Accept a new socket connection
void accept_new_socket(int server_socket) {
  bool accepted = false;
  for (size_t index = 0; index <= MAX_CLIENTS; index++) {
    if (!clients[index].active()) {
      clients[index].acceptConnection(server_socket);
      states[index] = State();
      accepted = true;
      break;
    }
  }
  if (!accepted) {
    ESP_LOGE(
        NETWORKING_LOG_TAG,
        "Failed to add the new client because we cant handle more connections");
  }
}

// Runs between each frame if there is something to do for a client
void process_client(int clientId) {}

int server_socket;

void initializeServer() {
  int ip_protocol = 0;

  struct sockaddr_storage dest_addr;

  struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
  dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
  dest_addr_ip4->sin_family = AF_INET;
  dest_addr_ip4->sin_port = htons(PORT);
  ip_protocol = IPPROTO_IP;

  int status = 0;
  server_socket = socket(AF_INET, SOCK_STREAM, ip_protocol);
  if (server_socket < 0) {
    ESP_LOGE(NETWORKING_LOG_TAG, "Unable to create socket: errno %d", errno);
    vTaskDelete(NULL);
    return;
  }
  int opt = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  status = fcntl(server_socket, F_SETFL,
                 fcntl(server_socket, F_GETFL, 0) | O_NONBLOCK);
  if (status == -1) {
    ESP_LOGE(NETWORKING_LOG_TAG, "Unable to set socket fd to nonblocking");
    return;
  }
  ESP_LOGI(NETWORKING_LOG_TAG, "Socket created");

  int err =
      bind(server_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err != 0) {
    ESP_LOGE(NETWORKING_LOG_TAG, "Socket unable to bind: errno %d", errno);
    close(server_socket);
  }
  ESP_LOGI(NETWORKING_LOG_TAG, "Socket bound, port %d", PORT);

  err = listen(server_socket, 1);
  if (err != 0) {
    ESP_LOGE(NETWORKING_LOG_TAG, "Error occurred during listen: errno %d",
             errno);
    close(server_socket);
  }
}

void processSocketEvents() {
  pollfd pollfds[1 + MAX_CLIENTS];
  pollfds[0].fd = server_socket;
  pollfds[0].events = POLLIN;
  pollfds[0].revents = 0;

  for (auto clientId = 0; clientId < MAX_CLIENTS; clientId++) {
    pollfds[clientId + 1] = clients[clientId].getPollFd();
  }

  auto poll_result = poll(pollfds, 1 + MAX_CLIENTS, 0);
  if (poll_result < 0) {
    ESP_LOGE(NETWORKING_LOG_TAG, "Failed to poll for new connections: errno %d",
             errno);
    return;
  }
  if (poll_result == 0) {
    // Nothing happened
    return;
  }

  ESP_LOGI(NETWORKING_LOG_TAG, "Something happened");
  // ESP_LOGI(WIFI_LOG_TAG, "Result not zero");

  if (pollfds[0].revents != 0) {
    ESP_LOGI(NETWORKING_LOG_TAG, "Accepting new connection");
    accept_new_socket(server_socket);
  }

  for (auto clientId = 0; clientId < MAX_CLIENTS; clientId++) {
    if (pollfds[clientId + 1].revents != 0) {
      ESP_LOGI(NETWORKING_LOG_TAG,
               "Something happened with connection %i. Lets process that",
               clientId);

      clients[clientId].receiveFrame();
    }
  }
}

void processNetworkEvents() {
  processSocketEvents();

  for (auto clientId = 0; clientId < MAX_CLIENTS; clientId++) {
    auto receivedData = clients[clientId].receiveBuffer();
    for (auto data : receivedData) {
      stepState(states[clientId], data);
    }
    clients[clientId].markBytesAsRead(receivedData.size_bytes());
  }

  for (auto clientId = 0; clientId < MAX_CLIENTS; clientId++) {
    if (clients[clientId].active()) {
      states[clientId].processScripts();
    }
  }

  for (auto clientId = 0; clientId < MAX_CLIENTS; clientId++) {
    clients[clientId].sendFromRingBuffer(states[clientId].sendingBuffer);
  }
}

void networkTestLoop(void *_) {
  initializeServer();
  while (1) {
    vTaskDelay(1);
    processNetworkEvents();
  }
  close(server_socket);
  vTaskDelete(NULL);
}

/// Start the tcp server in a new task
void startTcpServer(void) {
  // ESP_ERROR_CHECK(nvs_flash_init());
  // ESP_ERROR_CHECK(esp_netif_init());
  // ESP_ERROR_CHECK(esp_event_loop_create_default());

  /* This helper function configures Wi-Fi or Ethernet, as selected in
   * menuconfig. Read "Establishing Wi-Fi or Ethernet Connection" section in
   * examples/protocols/README.md for more information about this function.
   */
  // ESP_ERROR_CHECK(example_connect());

  // #ifdef CONFIG_EXAMPLE_IPV4

  xTaskCreate(networkTestLoop, "tcp_server", 4096, nullptr, 5, NULL);
  // #endif
  // #ifdef CONFIG_EXAMPLE_IPV6
  //     xTaskCreate(tcp_server_task, "tcp_server", 4096, (void*)AF_INET6, 5,
  //     NULL);
  // #endif
}