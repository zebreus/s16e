#include "../config.hpp"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <cstring>
#include <stdio.h>
#include <string.h>

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
const auto timeout_ms = 10000;

constexpr auto TAG = "s16e wifi";

static void eventHandler(void *arg, esp_event_base_t event_base,
                         int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
    esp_wifi_connect();
    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ESP_LOGI(TAG, "IP_EVENT_STA_GOT_IP");
    xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
  }
}

void setupWifi() {
  ESP_ERROR_CHECK(esp_netif_init());
  wifi_event_group = xEventGroupCreate();
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_event_handler_register(
      WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &eventHandler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &eventHandler, NULL));

  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());

  // esp_wifi_set_country_code("DE", true);

  wifi_config_t ap_config = {};
  memcpy(ap_config.ap.ssid, own_ssid, 32);
  ap_config.ap.ssid_len = strlen(own_ssid);
  ap_config.ap.authmode = WIFI_AUTH_OPEN;
  ap_config.ap.max_connection = 20;

  wifi_config_t sta_config = {};
  memcpy(sta_config.sta.ssid, foreign_ssid, 32);
  memcpy(sta_config.sta.password, foreign_password, 64);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_wifi_connect());
  int bits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdFALSE,
                                 pdTRUE, timeout_ms / portTICK_PERIOD_MS);
  ESP_LOGI("TAG", "bits=%x", bits);
  if (bits) {
    ESP_LOGI("TAG", "WIFI_MODE_STA connected.");
  } else {
    ESP_LOGI("TAG", "WIFI_MODE_STA can't connected.\n");
  }

  ESP_LOGI("wifi softAP",
           "wifi_init_softap finished. SSID:%s password:%s channel:%s",
           "EXAMPLE_ESP_WIFI_SSID", "EXAMPLE_ESP_WIFI_PASS",
           "EXAMPLE_ESP_WIFI_CHANNEL");

  // WiFi.disconnect();
  //  delay(1000);
  //   WiFi.softAPenableIPv6();

  // IPAddress localIP(192, 168, 90, 1);
  // IPAddress gateway(192, 168, 90, 1);
  // IPAddress subnet(255, 255, 255, 0);
  // WiFi.softAPConfig(localIP, gateway, subnet);
  // delay(2000);
  // WiFi.softAP(apSsid, NULL);
  // Serial.println("My IP on my own AP is:");
  // Serial.println(WiFi.softAPIP());
  // // Serial.println(WiFi.softAPIPv6());

  // Serial.println("Scanning WiFi networks");

  // // WiFi.scanNetworks will return the number of networks found
  // int n = SKIP_SCAN ? 0 : WiFi.scanNetworks();

  // Serial.println("I found ");
  // Serial.print(n);
  // Serial.println(" WiFi networks");

  // for (int i = 0; i < n; ++i) {
  //   // Print SSID and RSSI for each network found
  //   Serial.print(i + 1);
  //   Serial.print(": ");
  //   Serial.print(WiFi.SSID(i));
  //   Serial.print(" (");
  //   Serial.print(WiFi.RSSI(i));
  //   Serial.print(")");
  //   Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
  // }

  // bool connected = false;
  // const char *ssid = "Homeautomation";
  // //  #error Insert the WiFi password in the next line (and comment this
  // line) const char *password = "AutomationIsMagic"; for (int i = 0; i < n;
  // ++i) {
  //   if (WiFi.SSID(i) == ssid) {
  //     Serial.print("Hey, I know ");
  //     Serial.print(ssid);
  //     Serial.println("!");
  //     Serial.print("I am trying to talk it.");

  //     WiFi.disconnect();
  //     WiFi.begin(ssid, password);
  //     size_t attempts = 0;
  //     while (WiFi.status() != WL_CONNECTED && attempts++ <= 50) {
  //       Serial.print('.');
  //       delay(200);
  //     }
  //     Serial.println("");
  //     if (WiFi.status() == WL_CONNECTED) {
  //       Serial.println("That worked :D");
  //       Serial.print("Connected to ");
  //       Serial.println(ssid);
  //       break;
  //     }

  //     Serial.println("Seems like they dont want to talk to me");
  //   }
  // }

  // if (SKIP_SCAN) {
  //   Serial.println("Skipping wifi scan and connecting directly to speed up
  //   dev iterations"); WiFi.begin(ssid, password); size_t attempts = 0; while
  //   (WiFi.status() != WL_CONNECTED && attempts++ <= 50) {
  //     Serial.print('.');
  //     delay(100);
  //   }
  //   Serial.println("");
  // }

  // if (WiFi.status() != WL_CONNECTED) {
  //   Serial.println("I did not find any network that wants to communicate with
  //   me D:");
  //   // TODO: Open own AP now
  // }

  // Serial.println("Just FYI, my ipv4 is:");
  // Serial.println(WiFi.localIP());

  // // Wait a bit for no reason
  // server.begin();
}