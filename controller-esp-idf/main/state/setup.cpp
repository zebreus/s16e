// #include <WiFi.h>
// #include <lwip/sockets.h>
// #include <lwip/netdb.h>
// #include "octafont-regular.h"
// #include "octafont-bold.h"
#include "setup.hpp"
#include "../config.hpp"
#include "../display/Display.hpp"
#include "../display/setup.hpp"
#include "../helpers.hpp"
#include "../state/setup.hpp"
#include "../stats.hpp"
#include "State.hpp"
#include "driver/gpio.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <cassert>
#include <cstring>
#include <optional>

void printHelp(State &state) {
  constexpr char help[] =
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
  LUA <script>: Reads a lua script that will be executed every frame until the connection is closed. <script> is read until 512 bytes are read or until the string 'AUL' followed by a newline is send\n\
  SOL: Remove the current lua script. Does nothing if no script is loaded. \n\
\n\
Add new features at: https://github.com/zebreus/s16e\n";

  static_assert((sizeof(help) < SEND_QUEUE_SIZE),
                "Error: Send queue is not big enough to fit the help text");
  state.print({help, sizeof(help)});
}

void printColorAt(State &state, unsigned char x, unsigned char y) {
  // TODO: implement
  unsigned char value = display.getPixel(x, y);

  state.printf(100, "PX %i %i %02x%02x%02xff\n", x, y, value, value, value);
}

void printStats(State &state) {
  state.printf(400,
               "px: %llu\n"
               "up: %llu\n"
               "conn: %i\n"
               "max_conn: %llu\n"
               "haltezeit: %llu\n"
               "toggled: %llu\n"
               "cpu: %s\n",
               stats.pixelsDrawn, getTime(), stats.currentConnections,
               stats.maxConnections,
               stats.wagenHaltOnMillis +
                   (wagenHaltOn ? (getTime() - wagenHaltTurnedOnAt) : 0),
               stats.wagenHaltToggled, (const char *)CONFIG_IDF_TARGET);
}

bool wagenHaltOn = false;
unsigned long long wagenHaltTurnedOnAt = 0;

// template <typename F>
// void stepState(State& state, unsigned char c, F printer) {
void stepState(State &state, unsigned char c) {
  switch (state.state) {
  case STATE_IDLE: {
    if (c == '\n') {
      state.print("Type 'HELP' for help\n");
    }
    state.state = c == 'P'   ? STATE_P
                  : c == 'S' ? STATE_S
                  : c == 'W' ? STATE_W
                  : c == 'H' ? STATE_H
                  : c == 'T' ? STATE_T
                  : c == 'D' ? STATE_D
                  : c == 'R' ? STATE_R
                  : c == 'L' ? STATE_L
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
    unsigned char value = c;
    display.setPixel(state.nextX, state.nextY, value, 255);
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
        bool displayPixel = false;
        if (state.asciiColorIndex == 1) {
          displayPixel = true;
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
          displayPixel = true;
          state.alpha = 0xffu;
        }
        if (state.asciiColorIndex == 8) {
          displayPixel = true;
        }
        if (displayPixel) {
          display.setPixel(state.nextX, state.nextY, state.red, state.alpha);
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
                  : c == 'O' ? STATE_SO
                             : STATE_IDLE;
  } break;
  case STATE_SP: {
    state.print("SP is currently not supported, while the datastructure for "
                "the display is being reworked. Pester zebreus to fix this");
    // ((unsigned char *)allData)[state.spIndex] = c;
    // stats.pixelsDrawn += 8;

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
      state.printf(100, "SIZE %i %i\n", WIDTH, HEIGHT);
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
      printHelp(state);
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
      printStats(state);
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
      state.printf(50, "%llu\n", getTime());
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
    // LUA:
  case STATE_L: {
    state.state = c == 'U' ? STATE_LU : STATE_IDLE;
  } break;
  case STATE_LU: {
    if (c != 'A') {
      state.state = STATE_IDLE;
      break;
    }
    state.state = STATE_LUA;
    state.luaScriptContent = {0};
    state.luaScriptContentLength = 0;
    state.luaEndSequenceProgress = 0;
  } break;
  case STATE_LUA: {
    // Allow a bit extra to handle trailing UAL\n. The extra bytes will be
    // removed later
    if (state.luaScriptContentLength < LUA_MAX_SCRIPT_LENGTH) {
      state.luaScriptContent[state.luaScriptContentLength++] = c;
    } else {
      // Create
      state.state = STATE_IDLE;
    }
    if (state.luaScriptContentLength >= 4 &&
        state.luaScriptContent[state.luaScriptContentLength - 4] == 'A' &&
        state.luaScriptContent[state.luaScriptContentLength - 3] == 'U' &&
        state.luaScriptContent[state.luaScriptContentLength - 2] == 'L' &&
        state.luaScriptContent[state.luaScriptContentLength - 1] == '\n') {
      state.luaScriptContentLength -= 4;
      state.state = STATE_IDLE;
    }
    if (state.state == STATE_IDLE) {
      if (state.luaScriptContentLength == LUA_MAX_SCRIPT_LENGTH &&
          state.luaScriptContent[state.luaScriptContentLength - 3] == 'A' &&
          state.luaScriptContent[state.luaScriptContentLength - 2] == 'U' &&
          state.luaScriptContent[state.luaScriptContentLength - 1] == 'L') {
        state.luaScriptContentLength -= 3;
      }
      if (state.luaScriptContentLength == LUA_MAX_SCRIPT_LENGTH &&
          state.luaScriptContent[state.luaScriptContentLength - 2] == 'A' &&
          state.luaScriptContent[state.luaScriptContentLength - 1] == 'U') {
        state.luaScriptContentLength -= 2;
      }
      if (state.luaScriptContentLength == LUA_MAX_SCRIPT_LENGTH &&
          state.luaScriptContent[state.luaScriptContentLength - 1] == 'A') {
        state.luaScriptContentLength -= 1;
      }

      state.luaScriptContent[state.luaScriptContentLength] = 0;
      if (!state.luaScript.has_value()) {
        state.luaScript.emplace();
        state.luaScript->setOutput(state);
      }
      state.luaScript->loadScript(state.luaScriptContent.data());
    }
  } break;
  case STATE_SO: {
    state.state = c == 'L' ? STATE_SOL : STATE_IDLE;
  } break;
  case STATE_SOL: {
    state.state = STATE_IDLE;
    if (c != '\n') {
      break;
    }
    state.luaScript.reset();
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