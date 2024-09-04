// #include <WiFi.h>
// #include <lwip/sockets.h>
// #include <lwip/netdb.h>
// #include "octafont-regular.h"
// #include "octafont-bold.h"
#include "display/setup.hpp"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_event.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "networking/server.hpp"
#include "networking/setup.hpp"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "state/setup.hpp"
#include "stats.hpp"
#include <cstring>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

// hw_timer_t *timer = NULL;

// void turnOff0() {
//   gpio_set_level(rowPins[0], 1);
// }
// void turnOff1() {
//   gpio_set_level(rowPins[1], 1);
// }
// void turnOff2() {
//   gpio_set_level(rowPins[2], 1);
// }
// void turnOff3() {
//   gpio_set_level(rowPins[3], 1);
// }
// void turnOff4() {
//   gpio_set_level(rowPins[4], 1);
// }
// void turnOff5() {
//   gpio_set_level(rowPins[5], 1);
// }
// void turnOff6() {
//   gpio_set_level(rowPins[6], 1);
// }
// void turnOff7() {
//   gpio_set_level(rowPins[7], 1);

// }
// void setTurnOffInterrupt(unsigned char row) {
//  switch (row) {
//   case 0:
//    timerAttachInterrupt(timer, &turnOff0);
//    break;
//   case 1:
//    timerAttachInterrupt(timer, &turnOff1);
//    break;
//   case 2:
//    timerAttachInterrupt(timer, &turnOff2);
//    break;
//   case 3:
//    timerAttachInterrupt(timer, &turnOff3);
//    break;
//   case 4:
//    timerAttachInterrupt(timer, &turnOff4);
//    break;
//   case 5:
//    timerAttachInterrupt(timer, &turnOff5);
//    break;
//   case 6:
//    timerAttachInterrupt(timer, &turnOff6);
//    break;
//   case 7:
//    timerAttachInterrupt(timer, &turnOff7);
//    break;
//  }
// }

// void serialStep() {
//   unsigned int charsRemaining = MAX_CHARS_PER_PIXEL;
//   if (!isStateSleeping(serialState)){
//   while (Serial.available() && charsRemaining > 0) {
//     charsRemaining -= 1;
//     unsigned char c = Serial.read();
//     stepState(serialState, c, nullptr);
//   }
//   }
//   if (serialState.sendingText != nullptr) {
//   unsigned int sendChars = MAX_CHARS_PER_PIXEL;
//   if (serialState.charactersLeft < sendChars ){
//     sendChars = serialState.charactersLeft;
//   }
//   for (size_t i = 0; i < sendChars; i++) {
//     Serial.write(serialState.sendingText[i]);
//   }
//   serialState.sendingText = &serialState.sendingText[sendChars];
//   serialState.charactersLeft = serialState.charactersLeft - sendChars;
//   if (serialState.charactersLeft <= 0) {
//     serialState.sendingText = nullptr;
//   }

//   }
// }

// WiFiClient clients[MAX_CLIENTS];

// void networkStep() {
//   // Add new client if available
//   for (size_t index = 0 ; index < MAX_CLIENTS ; index++) {
//     auto client = clients[index];

//     auto& state = clientStates[index];

//     auto availableBytes = client.available();
//     if (availableBytes && !isStateSleeping(state)) {

//     auto readBytes  = 0;
//     while (readBytes < availableBytes && readBytes < MAX_CHARS_PER_PIXEL) {
//       readBytes++;
//       auto data = client.read();
//       stepState(clientStates[index], data, &client);
//       //      client.write(data);
//     }
//     }

//       if (state.sendingText != nullptr) {
//   unsigned int sendChars = MAX_CHARS_PER_PIXEL;
//   if (state.charactersLeft < sendChars ){
//     sendChars = state.charactersLeft;
//   }
//   if (sendChars > 0) {
//     char buffer[MAX_CHARS_PER_PIXEL + 1];
//     memcpy(buffer, state.sendingText, sendChars);
//     buffer[sendChars] = 0;
//     client.print((char*)buffer);
//     client.flush();
//   }
// //  for (size_t i = 0; i < sendChars; i++) {
// //
// //    client.write(state.sendingText[i]);
// //  }
//   state.sendingText = &state.sendingText[sendChars];
//   state.charactersLeft = state.charactersLeft - sendChars;
//   if (state.charactersLeft <= 0) {
//     state.sendingText = nullptr;
//   }

//   }

//     //client.flush();
//   }
// }

// const char* apSsid = "Strassenbahnanzeige";

// WiFiServer server(23);
// // TODO: Disabel when done with testing
// constexpr bool SKIP_SCAN = true;
// HardwareSerial Serials(2);

// // Periodice wifi maintainance. Runs between each frame
// void processWifi() {
//   // Add new client if available
//   WiFiClient client = server.accept();
//   if (client) {
//     int newClientIndex = -1;
//     if (client.connected()) {

//       for (size_t index = 0 ; index <= MAX_CLIENTS ; index++) {
//         if (!clients[index]) {
//           Serial.print("Connected to client ");
//           Serial.print(client.remoteIP());
//           Serial.print(" (");
//           Serial.print(index);
//           Serial.println(")");
//           client.setNoDelay(true);
//           {
//             Serial.println("Connected server success");
//             int keepAlive = 5000; // Milliseconds
//             int  keepIdle = 5000; // The time (in seconds) the connection
//             needs to remain idle before TCP starts sending keepalive probes,
//             if the socket option SO_KEEPALIVE has been set on this socket.
//             int  keepInterval = 5000; // The time (in seconds) between
//             individual keepalive probes. int  keepCount = 1; // The maximum
//             number of keepalive probes TCP should send before dropping the
//             connection.
// //            if (client.setSocketOption(SOL_SOCKET, SO_KEEPALIVE,
// (void*)&keepCount, sizeof(keepCount)))
// //              Serial.println("TCP_KEEPALIVE failed");
// //            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPALIVE,
// (void*)&keepAlive, sizeof(keepAlive)))
// //              Serial.println("TCP_KEEPALIVE failed");
// //            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPIDLE,
// (void*)&keepIdle, sizeof(keepIdle)))
// //              Serial.println("TCP_KEEPIDLE failed");
// //            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPINTVL,
// (void*)&keepInterval, sizeof(keepInterval)))
// //              Serial.println("TCP_KEEPINTVL failed");
// //            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPCNT,
// (void*)&keepCount, sizeof(keepCount)))
// //              Serial.println("TCP_KEEPCNT failed");
//           }
//           clients[index].stop();
//           clients[index] = client;
//           newClientIndex = index;
//           break;
//         }
//       }
//       if (newClientIndex == -1) {
//         Serial.print("Received connection from ");
//         Serial.print(client.remoteIP());
//         Serial.println(" but failed to add the new client because we cant
//         handle more connections");
//       }
//     }
//     if (newClientIndex == -1) {
//       client.stop();
//     }
//   }

//   unsigned int connectedClients = 0;
//   //bool non = true;
//   for (size_t index = 0 ; index < MAX_CLIENTS ; index++) {
//     if (!clients[index]) {
//       continue;
//     }

//     //    Serial.print(index);
//     //    Serial.print(": ");
//     //    Serial.print(clients[index].remoteIP());
//     //    Serial.print(": ");
//     //    Serial.print(clients[index].available());
//     //    Serial.println(": ");
//     //    non=false;

//     if (!clients[index].connected()) {
//       // Most disconnects are not logged here
//       Serial.print(clients[index].remoteIP());
//       Serial.println(" disconnected");
//       clients[index].stop();
//       clients[index] = WiFiClient();
//       continue;
//     }
//     connectedClients++;
//   }
//   stats.currentConnections = connectedClients;
//   if (connectedClients > stats.maxConnections){
//   stats.maxConnections = connectedClients;}
//   //  if(non){
//   //    Serial.println("no clients ");}
// }

// void loop()
// {

//   // WiFI housekeeping between frames
//   //  delay(1000);
//   //  auto before = getTime();

// //  for (int i = 0 ; i < 8; i++){
// //  for (int z = 0 ; z < 15; z++){
// //    allData [i][z] = 0xff;
// //    }
// //    }

// //  drawCharacter('%', 35);

// //  offset = (offset + 1) % WIDTH;
// //  drawColumn(0, offset - 1 + 120);

// //  writeColumn(0x7c, 35);
// //   writeColumn(0x12, 36);
// //    writeColumn(0x11, 37);
// //     writeColumn(0x12, 38);
// //      writeColumn(0x7c, 39);
// //for(size_t i = 0; i < 4 ; i++){
// //  processWifi();

//   //   auto after = getTime();
//   //
//   //   auto wifiDuration = middle - before;
//   //   auto displayDuration = after - middle;
//   //   Serial.print("wifi rate: ");
//   //   Serial.print(wifiDuration);
//   //   Serial.print("display rate: ");
//   //   Serial.println(displayDuration);

//   //  writeAllData();
//   //  gpio_set_level(LED_BUILTIN, 1);   // turn the LED on (HIGH is the
//   voltage level)
//   //  delay(1000);                       // wait for a second
//   //  gpio_set_level(LED_BUILTIN, 0);    // turn the LED off by making the
//   voltage LOW
//   //  delay(1000);
//   //  byte n = Serial.available();
//   //  if (n != 0)
//   //  {
//   //    byte m = Serial.readBytesUntil('\n', myData, sizeof (myData) - 1);
//   //    myData[m] = '\0';
//   //    Serial.println(myData);
//   //  }
//   //  Serial.println("Hello");
// }

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
  printf("Finished WiFi setup.\n");
  fflush(stdout);

  setupDisplay();
  printf("Finished Display setup.\n");
  fflush(stdout);

  size_t offset = 0;
  char message[101]; // max length you’ll need +1
  // TODO: Add IP
  snprintf(message, 100, "nc %s 23", "WiFi.localIP().toString().c_str()");
  drawString(message, offset);

  printf("Starting loop now.\n");
  fflush(stdout);

  // startTcpServer();
  initializeServer();
  while (true) {
    // //   // processWifi();
    vTaskDelay(10 / portTICK_PERIOD_MS);
    processNetworkEvents();
    displayFrame();
  }
}