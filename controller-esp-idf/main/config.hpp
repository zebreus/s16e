#pragma once

#include "soc/gpio_num.h"
#include <cstddef>

// ####   Display configuration  ####

constexpr unsigned char WIDTH = 120;
constexpr unsigned char HEIGHT = 8;
constexpr int SUBFRAMES = 5;
constexpr int SUBFRAMES_PER_FRAME = 3;

// Output enable low active
// #define OUTPUT_ENABLE ((gpio_num_t)4)
constexpr gpio_num_t OUTPUT_ENABLE = GPIO_NUM_4;
// Serial data
// #define DATA ((gpio_num_t)3)
constexpr gpio_num_t DATA = GPIO_NUM_3;
// Shift register clock
constexpr gpio_num_t SHIFT_REGISTER_CLOCK = GPIO_NUM_2;
// Register clock
constexpr gpio_num_t REGISTER_CLOCK = GPIO_NUM_1;
// Pins for multiplexing the rows
constexpr gpio_num_t ROW_0 = GPIO_NUM_5;
constexpr gpio_num_t ROW_1 = GPIO_NUM_6;
constexpr gpio_num_t ROW_2 = GPIO_NUM_7;
constexpr gpio_num_t ROW_3 = GPIO_NUM_8;
constexpr gpio_num_t ROW_4 = GPIO_NUM_9;
constexpr gpio_num_t ROW_5 = GPIO_NUM_10;
constexpr gpio_num_t ROW_6 = GPIO_NUM_20;
constexpr gpio_num_t ROW_7 = GPIO_NUM_21;
// Pin for the wagen halt signal
constexpr gpio_num_t WAGEN_HALT = GPIO_NUM_0;
constexpr gpio_num_t ROW_PINS[8] = {ROW_0, ROW_1, ROW_2, ROW_3,
                                    ROW_4, ROW_5, ROW_6, ROW_7};

// ####   Network configuration   ####

constexpr char own_ssid[32] = "Strassenbahnanzeige";
#error Insert the WiFi password in the next line (and comment this line)
// constexpr char foreign_ssid[32] = "Example";
// constexpr char foreign_password[64] = "Example";

// #### Networking configuration

constexpr const char *NETWORKING_LOG_TAG = "networking";
constexpr const char *RINGBUFFER_LOG_TAG = "ringbuffer";
constexpr const char *LUA_SCRIPTING_LOG_TAG = "lua";

constexpr size_t MAX_CLIENTS = 20;
// The max number of bytes that the server will receive per client per frame
constexpr size_t RECEIVE_BYTES_PER_FRAME = 130;
// The max number of bytes that the server will send to a client per frame
constexpr size_t SEND_BYTES_PER_FRAME = 130;
// The max queue of send bytes per client
constexpr size_t SEND_QUEUE_SIZE = 2000;

constexpr unsigned short PORT = 23;
constexpr int KEEPALIVE_IDLE = 5000;
constexpr int KEEPALIVE_INTERVAL = 5000;
constexpr int KEEPALIVE_COUNT = 1;

// #### LUA configuration

constexpr int LUA_MAX_SCRIPT_LENGTH = 512;
constexpr int LUA_MAX_INSTRUCTIONS_PER_FRAME = 5000;