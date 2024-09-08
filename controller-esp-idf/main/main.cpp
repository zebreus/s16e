#include "display/Display.hpp"
#include "display/setup.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "networking/server.hpp"
#include "networking/setup.hpp"
#include "nvs_flash.h"
#include <cstring>
#include <stdio.h>

extern "C" void app_main(void) {
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  setupWifi();
  ESP_LOGI("main", "Finished WiFi setup.");

  setupDisplay();
  ESP_LOGI("main", "Finished Display setup.");

  drawString("Connecting...", 0);

  initializeServer();

  // Wait for an IP address
  vTaskDelay(2000 / portTICK_PERIOD_MS);

  char message[101]; // max length you’ll need +1
  auto ownIp = getOwnIp();
  sprintf(message, "nc %s 23", ownIp);
  drawString(message, 0);
  ESP_LOGI("main", "%s", message);

  while (true) {
    // vTaskDelay(10 / portTICK_PERIOD_MS);
    display.show();
    processNetworkEvents();
  }
}