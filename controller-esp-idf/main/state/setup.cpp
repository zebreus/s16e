// #include <WiFi.h>
// #include <lwip/sockets.h>
// #include <lwip/netdb.h>
// #include "octafont-regular.h"
// #include "octafont-bold.h"
#include "setup.hpp"
#include "../config.hpp"
#include "../display/setup.hpp"
#include "../helpers.hpp"
#include "../state/setup.hpp"
#include "../stats.hpp"
#include "driver/gpio.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <cassert>
#include <cstring>
#include <stdio.h>
#include <string.h>

void statePrint(State &state, const char *text, size_t length) {
  if (length == -1) {
    length = strlen(text);
  }
  // char buf[1000];
  // memcpy(buf, text, length);
  // buf[length] = 0;
  state.sendingBuffer.write(std::span(text, length));
}
void statePrint(State &state, const char *text) { statePrint(state, text, -1); }
void statePrint(State &state, int number) {
  char a[100];
  auto length = sprintf(a, "%d", number);
  statePrint(state, a, length + 1);
}
void statePrintln(State &state, const char *text, size_t length) {
  statePrint(state, text, length);
  statePrint(state, "\n", 1);
}
void statePrintln(State &state, const char *text) {
  statePrintln(state, text, -1);
}
void statePrintln(State &state, int number) {
  statePrint(state, number);
  statePrint(state, "\n", 1);
}

const char help[] =
    "This display supports the common pixelflut commands and some own commands:\n\
Pixelflut commands:\n\
  HELP: Print this help message\n\
  SIZE: Returns the size of the visible canvas in pixel as SIZE <w> <h>\n\
  PX <x> <y> Return the current color of a pixel as PX <x> <y> <rrggbb>\n\
  PX <x> <y> <rrggbb(aa)>: Draw a single pixel at position (x, y) with the specified hex color code. ffffff is on; everything else is off.\n\
Light up the 'Wagen hält' indicator:\n\
  WHf: Activate the 'Wagen halt' indicator\n\
  WH0: Deactivate the 'Wagen halt' indicator\n\
Print some text:\n\
  TXT <text>: Display text\n\
Shorthands for PX:\n\
  PX <x> <y> 0: Set the pixel at position (x, y) to black\n\
  PX <x> <y> f: Set the pixel at position (x, y) to white\n\
Binary commands (No newline required):\n\
  PB<x><y><v>: Set the pixel at (x, y) to brightness <v>. <x>, <y> and <v> are one byte binary numbers.\n\
  SP: Update the whole image. Send SP and then a bitstream for the whole image\n\
Fancy commands (binary and untested) \n\
  ROT<x><y>: Rotate the buffer by <x> and <y>. <x> and <y> are one byte binary numbers\n\
  TI: Get the current time in millis as ascii string\n\
  SL <until>: Pause processing all further commands, until time is reached. Until is a time in millis as ascii string.\n\
\n\
Add new features at: https://github.com/zebreus/s16e\n";

static_assert((sizeof(help) < SEND_QUEUE_SIZE),
              "Error: Send queue is not big enough to fit the help text");

void printHelp(State &state) {
  // TODO: implement
  //   statePrintln(state, (char *)help);
}

void printColorAt(State &state, unsigned char x, unsigned char y) {
  // TODO: implement
  //   unsigned char value = allData[y][(x / 8)];
  //   auto relevantValue = value & ((1 << (7 - (x % 8))));
  //   bool active = relevantValue != 0;

  //   statePrint(state, "PX ");
  //   statePrint(state, x);
  //   statePrint(state, " ");
  //   statePrint(state, y);
  //   if (active) {
  //     statePrintln(state, " ffffff");
  //   } else {
  //     statePrintln(state, " 000000");
  //   }
}

bool wagenHaltOn = false;
unsigned long long wagenHaltTurnedOnAt = 0;

// template <typename F>
// void stepState(State& state, unsigned char c, F printer) {
void stepState(State &state, unsigned char c) {
  switch (state.state) {
  case STATE_IDLE: {
    if (c == '\n') {
      statePrintln(state, "Type 'HELP' for help");
    }
    state.state = c == 'P'   ? STATE_P
                  : c == 'S' ? STATE_S
                  : c == 'W' ? STATE_W
                  : c == 'H' ? STATE_H
                  : c == 'T' ? STATE_T
                  : c == 'D' ? STATE_D
                  : c == 'R' ? STATE_R
                             : STATE_IDLE;
  } break;
  case STATE_P: {
    state.state = c == 'B' ? STATE_PB : c == 'X' ? STATE_PX : STATE_IDLE;
  } break;

  // PB: Binary
  case STATE_PB: {
    state.nextX = c;
    state.state = state.nextX < WIDTH ? STATE_X : STATE_IDLE;
  } break;
  case STATE_X: {
    state.nextY = c;
    state.state = state.nextY < HEIGHT ? STATE_Y : STATE_IDLE;
  } break;
  case STATE_Y: {
    unsigned char oldValue = allData[state.nextY][(state.nextX / 8)];
    unsigned char value = (c == 0 || c == '0') ? 0 : 1;
    //        allData[state.nextY % HEIGHT][state.nextX % WIDTH] = value;
    if (value) {
      allData[state.nextY][(state.nextX / 8)] =
          oldValue | (1 << (7 - (state.nextX % 8)));
    } else {
      allData[state.nextY][(state.nextX / 8)] =
          oldValue & (~(1 << (7 - (state.nextX % 8))));
    }
    stats.pixelsDrawn += 1;
    state.state = STATE_IDLE;
  } break;

  // PX: Ascii
  case STATE_PX: {
    state.state = c == ' ' ? STATE_PX_SPACE : STATE_IDLE;
  } break;
  case STATE_PX_SPACE: {
    unsigned char number = c - 48;
    if (number >= 10) {
      state.state = STATE_IDLE;
    } else {
      state.nextX = number;
      state.state = STATE_PX_X1;
    }

  } break;
  case STATE_PX_X1: {
    if (c == ' ') {
      state.state = STATE_PX_X_SPACE;
      break;
    }
    unsigned char number = c - 48;
    if (number >= 10) {
      state.state = STATE_IDLE;
    } else {
      state.nextX = number + state.nextX * 10;
      state.state = STATE_PX_X2;
    }

  } break;
  case STATE_PX_X2: {
    if (c == ' ') {
      state.state = STATE_PX_X_SPACE;
      break;
    }
    unsigned char number = c - 48;
    if (number >= 10) {
      state.state = STATE_IDLE;
    } else {
      state.nextX = number + state.nextX * 10;
      state.state = STATE_PX_X3;
    }

  } break;
  case STATE_PX_X3: {
    state.state = c == ' ' ? STATE_PX_X_SPACE : STATE_IDLE;
  } break;
  case STATE_PX_X_SPACE: {
    unsigned char number = c - 48;
    if (number >= 10) {
      state.state = STATE_IDLE;
    } else {
      state.nextY = number;
      state.state = STATE_PX_Y;
    }

  } break;
  case STATE_PX_Y: {
    if (state.nextX >= WIDTH || state.nextY >= HEIGHT) {
      state.state = STATE_IDLE;
      break;
    }

    if (c == '\n') {
      printColorAt(state, state.nextX, state.nextY);
      state.state = STATE_IDLE;
      break;
    }

    if (c == ' ') {

      state.state = STATE_PX_Y_SPACE;
      state.red = 0;
      state.green = 0;
      state.alpha = 0;
      state.blue = 0;
      state.asciiColorIndex = 0;

      break;
    }
    state.state = STATE_IDLE;
  } break;
  case STATE_PX_Y_SPACE: {
    if (c >= 'A' && c <= 'F') {
      // To lowercase
      c += 32;
    }

    bool cIsHex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    if (!cIsHex) {
      if (c == '\n') {
        bool display = false;
        if (state.asciiColorIndex == 1) {
          display = true;

          if (state.red == 0xf0u) {

            state.red = 0xffu;
            state.green = 0xffu;
            state.blue = 0xffu;
            state.alpha = 0xffu;
          } else {
            state.red = 0x00u;
            state.green = 0x00u;
            state.blue = 0x00u;
            state.alpha = 0x00u;
          }
        }
        if (state.asciiColorIndex == 6) {
          display = true;
          state.alpha = 0xffu;
        }
        if (state.asciiColorIndex == 8) {
          display = true;
        }
        if (display) {
          unsigned char oldValue = allData[state.nextY][(state.nextX / 8)];
          if (state.red == 0xffu && state.green == 0xffu &&
              state.blue == 0xffu && state.alpha == 0xffu) {
            allData[state.nextY][(state.nextX / 8)] =
                oldValue | (1 << (7 - (state.nextX % 8)));
          } else {
            allData[state.nextY][(state.nextX / 8)] =
                oldValue & (~(1 << (7 - (state.nextX % 8))));
          }
          stats.pixelsDrawn += 1;
        }
      }
      state.state = STATE_IDLE;
      break;
    }

    unsigned char nibble =
        (c >= '0' && c <= '9') ? (c - '0') : ((c - 'a') + 0xa);
    switch (state.asciiColorIndex++) {
    case 0:
      state.red = nibble << 4;
      break;
    case 1:
      state.red |= nibble;
      break;
    case 2:
      state.green = nibble << 4;
      break;
    case 3:
      state.green |= nibble;
      break;
    case 4:
      state.blue = nibble << 4;
      break;
    case 5:
      state.blue |= nibble;
      break;
    case 6:
      state.alpha = nibble << 4;
      break;
    case 7:
      state.alpha |= nibble;
      break;
    default:
      // too many nibbles; no command left
      state.state = STATE_IDLE;
    }
  } break;

  // SP: Full update binary
  case STATE_S: {
    state.spIndex = 0;
    state.state = c == 'P'   ? STATE_SP
                  : c == 'I' ? STATE_SI
                  : c == 'T' ? STATE_ST
                  : c == 'L' ? STATE_SL
                             : STATE_IDLE;
  } break;
  case STATE_SP: {
    ((unsigned char *)allData)[state.spIndex] = c;
    stats.pixelsDrawn += 8;

    if (state.spIndex == ((HEIGHT * (WIDTH / 8)) - 1)) {
      state.state = STATE_IDLE;
    } else {
      state.state = STATE_SP;
      state.spIndex += 1;
    }
  } break;

  // WH: Wagen hält (WH1 = on; WH0 = off)
  case STATE_W: {
    state.state = c == 'H' ? STATE_WH : STATE_IDLE;
  } break;
  case STATE_WH: {
    if (c >= 'A' && c <= 'F') {
      // To lowercase
      c += 32;
    }
    bool newWagenHalt = c == 'f';
    if (wagenHaltOn != newWagenHalt) {
      stats.wagenHaltToggled += 1;
      if (newWagenHalt) {
        // Turned on
        wagenHaltTurnedOnAt = getTime();
      } else {
        stats.wagenHaltOnMillis += getTime() - wagenHaltTurnedOnAt;
      }
    }
    wagenHaltOn = newWagenHalt;
    gpio_set_level(WAGEN_HALT, wagenHaltOn ? 1 : 0);
    state.state = STATE_IDLE;
  } break;

  // SIZE:
  case STATE_SI: {
    state.state = c == 'Z' ? STATE_SIZ : STATE_IDLE;
  } break;
  case STATE_SIZ: {
    state.state = c == 'E' ? STATE_SIZE : STATE_IDLE;
  } break;
  case STATE_SIZE: {
    if (c == '\n') {
      statePrint(state, (const char *)"SIZE ");
      statePrint(state, WIDTH);
      statePrint(state, " ");
      statePrintln(state, HEIGHT);
    }
    state.state = STATE_IDLE;
  } break;

  // HELP:
  case STATE_H: {
    state.state = c == 'E' ? STATE_HE : STATE_IDLE;
  } break;
  case STATE_HE: {
    state.state = c == 'L' ? STATE_HEL : STATE_IDLE;
  } break;
  case STATE_HEL: {
    state.state = c == 'P' ? STATE_HELP : STATE_IDLE;
  } break;
  case STATE_HELP: {
    if (c == '\n') {
      statePrint(state, help, sizeof(help));
    }
    state.state = STATE_IDLE;
  } break;

  // STATS:
  case STATE_ST: {
    state.state = c == 'A' ? STATE_STA : STATE_IDLE;
  } break;
  case STATE_STA: {
    state.state = c == 'T' ? STATE_STAT : STATE_IDLE;
  } break;
  case STATE_STAT: {
    state.state = c == 'S' ? STATE_STATS : STATE_IDLE;
  } break;
  case STATE_STATS: {
    if (c == '\n') {
      struct Stats {
        unsigned long long pixelsDrawn = 0;
        unsigned long long wagenHaltToggled = 0;
        unsigned long long wagenHaltOnMillis = 0;
        unsigned long long maxConnections = 0;
        unsigned long long currentConnections = 0;
      };
      statePrint(state, "px:");
      statePrintln(state, stats.pixelsDrawn);
      statePrint(state, "up:");
      statePrintln(state, getTime());
      statePrint(state, "conn:");
      statePrintln(state, stats.currentConnections);
      statePrint(state, "max_conn:");
      statePrintln(state, stats.maxConnections);
      statePrint(state, "haltezeit:");
      statePrintln(state,
                   stats.wagenHaltOnMillis +
                       (wagenHaltOn ? (getTime() - wagenHaltTurnedOnAt) : 0));
      statePrint(state, "toggled:");
      statePrintln(state, stats.wagenHaltToggled);
      statePrint(state, "cpu:");
      statePrintln(state, CONFIG_IDF_TARGET);
    }
    state.state = STATE_IDLE;
  } break;

  // TXT:
  case STATE_T: {
    state.state = c == 'X' ? STATE_TX : c == 'I' ? STATE_TI : STATE_IDLE;
  } break;
  case STATE_TX: {
    state.state = c == 'T' ? STATE_TXT : STATE_IDLE;
  } break;
  case STATE_TXT: {
    state.textPosition = 0;
    state.state = c == ' ' ? STATE_TXT_SPACE : STATE_IDLE;
  } break;
  case STATE_TXT_SPACE: {
    if (c == 0 || c == '\n') {
      // TODO: get_width == -1
      state.state = STATE_IDLE;
      break;
    }
    auto width = drawCharacter(c, state.textPosition);
    state.textPosition += width;
    drawColumn(0, state.textPosition);
    state.textPosition += 1;

  } break;
  // ROT:
  case STATE_R: {
    state.state = c == 'O' ? STATE_RO : STATE_IDLE;
  } break;
  case STATE_RO: {
    state.state = c == 'T' ? STATE_ROT : STATE_IDLE;
  } break;

  // PB: Binary
  case STATE_ROT: {
    state.nextX = c;
    state.state = STATE_ROT_X;
  } break;
  case STATE_ROT_X: {
    state.nextY = c;
    state.state = STATE_ROT_Y;
  } break;
  case STATE_ROT_Y: {
    if (c == '\n') {
      rotate(state.nextX, state.nextY);
    }
    state.state = STATE_IDLE;
  } break;

  // TI: Print the current time
  case STATE_TI: {
    if (c == '\n') {
      char timeBuffer[100] = {0};

      sprintf(timeBuffer, "%llu\n", getTime());
      statePrintln(state, (const char *)timeBuffer);
    }
    state.state = STATE_IDLE;
  } break;

  // SL: Wait with processing further commands until the given time
  case STATE_SL: {
    if (c == ' ') {
      state.sleepUntil = 0;
      break;
    }
    if (c == '\n') {
      state.state = STATE_IDLE;
      state.sleep = true;
      break;
    }
    unsigned char number = c - 48;
    if (number >= 10) {
      state.state = STATE_IDLE;
    } else {
      state.sleepUntil = number + state.sleepUntil * 10;
    }

  } break;
  default:
    state.state = STATE_IDLE;
  }
}

bool isStateSleeping(State &state) {
  if (state.sleep && state.sleepUntil < getTime()) {
    state.sleep = false;
  }
  return state.sleep;
}