#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "octafont-regular.h"
#include "octafont-bold.h"
#include "freertos/task.h"

// Output enable low active
#define OUTPUT_ENABLE 4
// Serial data
#define DATA 3
// Shift register clock
#define SHIFT_REGISTER_CLOCK 2
// Register clock
#define REGISTER_CLOCK 1

#define SLOWDOWN



#define ROW_0 5
#define ROW_1 6
#define ROW_2 7
#define ROW_3 8
#define ROW_4 9
#define ROW_5 10
#define ROW_6 20
#define ROW_7 21

#define WAGEN_HALT 0

constexpr unsigned char WIDTH = 120;
constexpr unsigned char HEIGHT = 8;

//unsigned char allData[HEIGHT][WIDTH/8] = {
//  {0b10000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000},
//  {0b10000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b00000000, 0b00000000},
//  {0b10000000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b00000000},
//  {0b10000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000},
//  {0b10000000, 0b00001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000000, 0b00000000},
//  {0b10000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001000, 0b00000000, 0b00000000},
//  {0b10000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b00000000, 0b00000000},
//  {0b10000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000},
//};

//unsigned char allData[HEIGHT][WIDTH/8] = {
//  {0b00000000, 0b11111110, 0b00000000, 0b00011100, 0b11111111, 0b00000000, 0b11111111, 0b10000000, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b11100000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b00100010, 0b10000000, 0b00000000, 0b10000000, 0b10000000, 0b10000001, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b01000010, 0b10000000, 0b00000000, 0b10000000, 0b10000000, 0b10000001, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b01000010, 0b10000000, 0b00000000, 0b11111111, 0b10000000, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b11111110, 0b00000000, 0b00000000},
//  {0b00000000, 0b11111110, 0b00000000, 0b01000010, 0b11111111, 0b00000000, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b01000010, 0b10000000, 0b00000000, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b00100010, 0b10000000, 0b00000000, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000000},
//  {0b00000000, 0b11111110, 0b00000000, 0b00011100, 0b11111111, 0b00000000, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000, 0b11111110, 0b00000000, 0b00000000}
//};

//unsigned char allData[8][15] = {
//  {0b00000000, 0b01111110, 0b00000000, 0b00001100, 0b00000000, 0b00000000, 0b01111100, 0b00000000, 0b01111100, 0b00000000, 0b00011110, 0b00000000, 0b01111110, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b00010010, 0b00000000, 0b00000000, 0b10000010, 0b00000000, 0b10000010, 0b00000000, 0b01100010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000000, 0b00000000, 0b00100010, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b10000000, 0b00000000, 0b11000010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
//  {0b00000000, 0b01111100, 0b00000000, 0b00100000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b01111100, 0b00000000, 0b00000010, 0b00000000, 0b01111100, 0b00000000, 0b00000000},
//  {0b00000000, 0b00000010, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000010, 0b00000000, 0b00000010, 0b00000000, 0b00000010, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b00010010, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b10000010, 0b00000000, 0b00000010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
//  {0b00000000, 0b10000010, 0b00000000, 0b00001100, 0b00000000, 0b00000000, 0b10000010, 0b00000000, 0b10000010, 0b00000000, 0b00000010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
//  {0b00000000, 0b01111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01111100, 0b00000000, 0b01111100, 0b00000000, 0b00011110, 0b00000000, 0b01111100, 0b00000000, 0b00000000}
//};

unsigned char allData[8][15];


constexpr size_t rowPins[8] =  { ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7 };



#define STATE_IDLE 0
#define STATE_P 1
#define STATE_PB 2
#define STATE_X 3
#define STATE_Y 4
#define STATE_VALUE 5
#define STATE_PX 6
#define STATE_PX_SPACE 7
#define STATE_PX_X1 8
#define STATE_PX_X2 9
#define STATE_PX_X3 10
#define STATE_PX_X_SPACE 11
#define STATE_PX_Y 12
#define STATE_PX_Y_SPACE 13
#define STATE_S 14
#define STATE_SP 15
#define STATE_W 16
#define STATE_WH 17
#define STATE_SI 18
#define STATE_SIZ 19
#define STATE_SIZE 20
#define STATE_H 21
#define STATE_HE 22
#define STATE_HEL 23
#define STATE_HELP 24
#define STATE_ST 25
#define STATE_STA 26
#define STATE_STAT 27
#define STATE_STATS 28
#define STATE_T 29
#define STATE_TX 30
#define STATE_TXT 31
#define STATE_D 32
#define STATE_DE 33
#define STATE_DEV 34
#define STATE_TXT_SPACE 35
#define STATE_R 36
#define STATE_RO 37
#define STATE_ROT 38
#define STATE_ROT_X 39
#define STATE_ROT_Y 40
#define STATE_TI 41
#define STATE_SL 42
#define STATE_SL_NUM 42

struct State {
  unsigned char state = STATE_IDLE;
  unsigned char nextX = 0;
  unsigned char nextY = 0;
  size_t spIndex = 0;
  size_t asciiColorIndex = 0;
  unsigned char red = 0;
  unsigned char green = 0;
  unsigned char blue = 0;
  unsigned char alpha = 0;
  size_t textPosition = 0;

  // The text that is currently been send
  char* sendingText = nullptr;
  // A buffer to use for sendingText
  char sendingBuffer[100];
  // The number of characters that are left
  size_t charactersLeft = 0;

  // Sleep until this time when sleep is set
  unsigned int sleepUntil = 0;
  bool sleep = false;
  
};

State serialState;



void printGreeting(WiFiClient* client) {
  Serial.println("Welcome to");
  Serial.println("   ____  _  __      ");
  Serial.println("/ ___|/ |/ /_   ___ ");
  Serial.println("\\___ \\| | '_ \\ / _ \\");
  delay(10);
  Serial.println(" ___) | | (_) |  __/");
  Serial.println("|____/|_|\\___/ \\___|");
  Serial.println("");
  printHelp(client);
  Serial.println("");
  Serial.println("Thank you for displaying with S16e");
  delay(10);
}

// Templates seem to produce weird nonfunctional binaries, so I print using this
void clientPrintln(WiFiClient* client, char* content) {
  if (client != nullptr) {
    client->println(content);
  } else {
    Serial.println(content);
  }
}
void clientPrintln(WiFiClient* client, int content) {
  if (client != nullptr) {
    client->println(content);
  } else {
    Serial.println(content);
  }
}
void clientPrint(WiFiClient* client, char* content) {
  if (client != nullptr) {
    client->print(content);
  } else {
    Serial.print(content);
  }
}
void clientPrint(WiFiClient* client, int content) {
  if (client != nullptr) {
    client->print(content);
  } else {
    Serial.print(content);
  }
}

const char help[] = "This display supports the common pixelflut commands and some own commands:\n\
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

void printHelp(WiFiClient* client) {
  clientPrintln(client, (char*)help);
}

//template <typename F>
//void printHelp(F printer){
//  printer.println("This display supports some pixelflut commands and some own commands. The resolution is 128x8. ");
//  printer.println("Here is a overview of the supported commands:");
//  delay(10);
//  printer.println("HELP: Print this help message");
//  printer.println("SIZE: Returns the size of the visible canvas in pixel as SIZE <w> <h>");
//  printer.println("PX XXX Y f: Activate the pixel at the supplied X and Y coordinates. X and Y are ascii numbers");
//  printer.println("PX XXX Y 0: Deactivate the pixel at the supplied X and Y coordinates. X and Y are ascii numbers");
//  delay(10);
//  printer.println("PBXY1: Activate the pixel at the supplied X and Y coordinates. X and Y are one byte binary numbers");
//  delay(10);
//  Serial.println("PBXY0: Deactivate the pixel at the supplied X and Y coordinates. X and Y are one byte binary numbers");
//  delay(10);
//  printer.println("WH1: Activate the 'Wagen halt' indicator");
//  printer.println("WH0: Deactivate the 'Wagen halt' indicator");
//  printer.println("SP: Update the whole image. Send SP and then a bitstream for the whole image");
//}

void printColorAt(WiFiClient* client, unsigned char x, unsigned char y) {
  unsigned char value = allData[y][(x / 8)];
  auto relevantValue = value & ((1 << (7 - (x % 8))));
  bool active = relevantValue != 0;

  clientPrint(client, "PX ");
  clientPrint(client, x);
  clientPrint(client, " ");
  clientPrint(client, y);
  if (active) {
    clientPrintln(client, " ffffff");
  } else {
    clientPrintln(client, " 000000");
  }
}

// This gets buggy sometimes
void rotate(unsigned char x, unsigned char y){
    // TODO: Make this more efficient
    for (int ii = 0; ii<(x % WIDTH); ii++){
    for (int row = 0; row < HEIGHT; row++){
//      char first = allData[col] >> 1 | allData[(col+1) % WIDTH] << 7;
      char last = (allData[row][0] >> 1) | (allData[row][(WIDTH/8) - 1] <<7);
      for (int col = (WIDTH/8) - 1; col > 0; col--){
        allData[row][col] = (allData[row][col] >> 1) | (allData[row][(col - 1)] <<7);
      }
      allData[row][0] = last;
  }}

  // TODO: Make this more efficient
  for (size_t ii = 0; ii < (y % HEIGHT); ii++){
  char tmpRow[WIDTH/8];
  memcpy(tmpRow, allData[(1) % HEIGHT], WIDTH/8);
  for (size_t i = 1; i < HEIGHT; i++){
    memcpy(allData[i], allData[(i + 1) % HEIGHT], WIDTH/8);
  }
  memcpy(allData[0], tmpRow, WIDTH/8);
  }
}

bool wagenHaltOn = false;
unsigned long long wagenHaltTurnedOnAt = 0;

struct Stats {
  unsigned long long pixelsDrawn = 0;
  unsigned long long wagenHaltToggled = 0;
  unsigned long long wagenHaltOnMillis = 0;
  unsigned long long maxConnections = 0;
  unsigned long long currentConnections = 0;
};
Stats stats;

//template <typename F>
//void stepState(State& state, unsigned char c, F printer) {
void stepState(State& state, unsigned char c, WiFiClient* client) {
  switch (state.state) {
    case STATE_IDLE: {
        if (c == '\n') {
          clientPrintln(client, 
          "Type 'HELP' for help");
          }
        state.state = c == 'P' ? STATE_P : c == 'S' ? STATE_S : c == 'W' ? STATE_W : c == 'H' ? STATE_H : c == 'T' ? STATE_T : c == 'D' ? STATE_D : c == 'R' ? STATE_R : STATE_IDLE;
      }
      break;
    case STATE_P: {
        state.state = c == 'B' ? STATE_PB : c == 'X' ? STATE_PX : STATE_IDLE;
      }
      break;

    // PB: Binary
    case STATE_PB: {
        state.nextX = c;
        state.state = state.nextX < WIDTH ? STATE_X : STATE_IDLE;
      }
      break;
    case STATE_X: {
        state.nextY = c;
        state.state = state.nextY < HEIGHT ? STATE_Y : STATE_IDLE;
      }
      break;
    case STATE_Y: {
      unsigned char oldValue = allData[state.nextY][(state.nextX / 8)];
        unsigned char value = (c == 0 || c == '0') ? 0 : 1;
//        allData[state.nextY % HEIGHT][state.nextX % WIDTH] = value;
            if  (value ) {
                allData[state.nextY][(state.nextX / 8)] = oldValue | (1 << (7 - (state.nextX % 8)));
              } else {
                allData[state.nextY][(state.nextX / 8)] = oldValue & (~(1 << (7 - (state.nextX % 8))));
              }
        stats.pixelsDrawn += 1;
        state.state = STATE_IDLE;
      }
      break;

    // PX: Ascii
    case STATE_PX: {
        state.state = c == ' ' ? STATE_PX_SPACE : STATE_IDLE;
      }
      break;
    case STATE_PX_SPACE: {
        unsigned char number = c - 48;
        if (number >= 10) {
          state.state = STATE_IDLE;
        } else {
          state.nextX = number;
          state.state = STATE_PX_X1;
        }

      }
      break;
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

      }
      break;
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

      }
      break;
    case STATE_PX_X3: {
        state.state = c == ' ' ? STATE_PX_X_SPACE : STATE_IDLE;
      }
      break;
    case STATE_PX_X_SPACE: {
        unsigned char number = c - 48;
        if (number >= 10) {
          state.state = STATE_IDLE;
        } else {
          state.nextY = number;
          state.state = STATE_PX_Y;
        }

      }
      break;
    case STATE_PX_Y: {
        if (state.nextX >= WIDTH || state.nextY >= HEIGHT) {
          state.state = STATE_IDLE;
          break;
        }

        if (c == '\n') {
          printColorAt(client, state.nextX, state.nextY);
          state.state = STATE_IDLE;
          break;
        }

        if (c == ' ') {

          state.state =  STATE_PX_Y_SPACE;
          state.red = 0;
          state.green = 0;
          state.alpha = 0;
          state.blue = 0;
          state.asciiColorIndex = 0;

          break;
        }
        state.state = STATE_IDLE;
      }
      break;
    case STATE_PX_Y_SPACE: {
        if (c >= 'A' && c <= 'F') {
          // To lowercase
          c += 32;
        }

        bool cIsHex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ;
        if (!cIsHex) {
          if (c == '\n') {
            bool display = false;
            if (state.asciiColorIndex == 1  ) {
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
              if  (state.red == 0xffu && state.green == 0xffu && state.blue == 0xffu && state.alpha == 0xffu ) {
                allData[state.nextY][(state.nextX / 8)] = oldValue | (1 << (7 - (state.nextX % 8)));
              } else {
                allData[state.nextY][(state.nextX / 8)] = oldValue & (~(1 << (7 - (state.nextX % 8))));
              }
              stats.pixelsDrawn += 1;
            }
          }
          state.state = STATE_IDLE;
          break;
        }


        unsigned char nibble = (c >= '0' && c <= '9') ? (c - '0') : ((c - 'a') + 0xa);
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
      }
      break;

    // SP: Full update binary
    case STATE_S: {
        state.spIndex = 0;
        state.state = c == 'P' ? STATE_SP : c == 'I' ? STATE_SI :  c == 'T' ? STATE_ST : c == 'L' ? STATE_SL : STATE_IDLE;
      }
      break;
    case STATE_SP: {
        ((unsigned char*)allData)[state.spIndex] = c;
        stats.pixelsDrawn += 8;

        if (state.spIndex == ((HEIGHT * (WIDTH / 8)) - 1)) {
          state.state = STATE_IDLE;
        } else {
          state.state = STATE_SP;
          state.spIndex += 1;
        }
      }
      break;

    // WH: Wagen hält (WH1 = on; WH0 = off)
    case STATE_W: {
        state.state = c == 'H' ? STATE_WH : STATE_IDLE;
      }
      break;
    case STATE_WH: {
        if (c >= 'A' && c <= 'F') {
          // To lowercase
          c += 32;
        }
        bool newWagenHalt = c == 'f';
        if (wagenHaltOn != newWagenHalt) {
          stats.wagenHaltToggled += 1;
          if (newWagenHalt ) {
            // Turned on
            wagenHaltTurnedOnAt = millis();
          } else {
            stats.wagenHaltOnMillis += millis() - wagenHaltTurnedOnAt;
          }
        }
        wagenHaltOn = newWagenHalt;
        digitalWrite(WAGEN_HALT, wagenHaltOn ? HIGH : LOW);
        state.state = STATE_IDLE;
      }
      break;

    // SIZE:
    case STATE_SI: {
        state.state = c == 'Z' ? STATE_SIZ : STATE_IDLE;
      }
      break;
    case STATE_SIZ: {
        state.state = c == 'E' ? STATE_SIZE : STATE_IDLE;
      }
      break;
    case STATE_SIZE: {
        if (c == '\n') {
          clientPrint(client, "SIZE ");
          clientPrint(client, WIDTH);
          clientPrint(client, " ");
          clientPrintln(client, HEIGHT);
        }
        state.state = STATE_IDLE;
      }
      break;

    // HELP:
    case STATE_H: {
        state.state = c == 'E' ? STATE_HE : STATE_IDLE;
      }
      break;
    case STATE_HE: {
        state.state = c == 'L' ? STATE_HEL : STATE_IDLE;
      }
      break;
    case STATE_HEL: {
        state.state = c == 'P' ? STATE_HELP : STATE_IDLE;
      }
      break;
    case STATE_HELP: {
        if (c == '\n') {
          state.charactersLeft = sizeof(help);
          state.sendingText = (char*)help;
        }
        state.state = STATE_IDLE;
      }
      break;

    // STATS:
    case STATE_ST: {
        state.state = c == 'A' ? STATE_STA : STATE_IDLE;
      }
      break;
    case STATE_STA: {
        state.state = c == 'T' ? STATE_STAT : STATE_IDLE;
      }
      break;
    case STATE_STAT: {
        state.state = c == 'S' ? STATE_STATS : STATE_IDLE;
      }
      break;
    case STATE_STATS: {
        if (c == '\n') {
          struct Stats {
            unsigned long long pixelsDrawn = 0;
            unsigned long long wagenHaltToggled = 0;
            unsigned long long wagenHaltOnMillis = 0;
            unsigned long long maxConnections = 0;
            unsigned long long currentConnections = 0;
          };
          clientPrint(client, "px:");
          clientPrintln(client, stats.pixelsDrawn);
          clientPrint(client, "up:");
          clientPrintln(client, millis());
          clientPrint(client, "conn:");
          clientPrintln(client, stats.currentConnections);
          clientPrint(client, "max_conn:");
          clientPrintln(client, stats.maxConnections);
          clientPrint(client, "haltezeit:");
          clientPrintln(client, stats.wagenHaltOnMillis + (wagenHaltOn ? (millis() - wagenHaltTurnedOnAt) : 0));
          clientPrint(client, "toggled:");
          clientPrintln(client, stats.wagenHaltToggled);
        }
        state.state = STATE_IDLE;
      }
      break;

      
    // TXT:
    case STATE_T: {
        state.state = c == 'X' ? STATE_TX : c == 'I' ? STATE_TI : STATE_IDLE;
      }
      break;
    case STATE_TX: {
        state.state = c == 'T' ? STATE_TXT : STATE_IDLE;
      }
      break;
   case STATE_TXT: {
    state.textPosition = 0;
        state.state = c == ' ' ? STATE_TXT_SPACE : STATE_IDLE;
      }
      break;
     case STATE_TXT_SPACE: {
        if (c == 0 || c == '\n') {
          //TODO: get_width == -1
          state.state = STATE_IDLE; 
          break;
         }
         auto width = drawCharacter(c, state.textPosition);
         state.textPosition += width;
         drawColumn(0, state.textPosition);
         state.textPosition += 1;
         
      }
      break;
    // ROT:
    case STATE_R: {
        state.state = c == 'O' ? STATE_RO : STATE_IDLE;
      }
      break;
    case STATE_RO: {
        state.state = c == 'T' ? STATE_ROT : STATE_IDLE;
      }
      break;

    // PB: Binary
    case STATE_ROT: {
        state.nextX = c;
        state.state = STATE_ROT_X;
      }
      break;
    case STATE_ROT_X: {
        state.nextY = c;
        state.state = STATE_ROT_Y;
      }
      break;
     case STATE_ROT_Y: {
        if (c == '\n'){
          rotate(state.nextX, state.nextY);
        }
        state.state = STATE_IDLE;
      }
      break;

    default:
      state.state = STATE_IDLE;
    // TI: Print the current time
    case STATE_TI: {
      if (c == '\n'){
        sprintf(state.sendingBuffer, "%d\n", millis());
        state.sendingText = (char*)state.sendingBuffer;
        state.charactersLeft = strlen(state.sendingBuffer);
      }
      state.state = STATE_IDLE;
     }
     break;

    // SL: Wait with processing further commands until the given time
    case STATE_SL: {
      if (c == ' '){
        state.sleepUntil = 0;
        break;
      }
      if (c == '\n'){
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
      
     }
     break;
  }


}


#define SPEED 0

// Write 8 bits
void writeChar(unsigned char data) {
  for (size_t index = 0; index < 8 ; index++) {
    digitalWrite(SHIFT_REGISTER_CLOCK, LOW);
    SLOWDOWN
    digitalWrite(DATA, (data & (1 << index)) ? HIGH : LOW);
    SLOWDOWN
    digitalWrite(SHIFT_REGISTER_CLOCK, HIGH);
    SLOWDOWN
//    digitalWrite(DATA, (data & (1 << index)) ? LOW : HIGH);
//    SLOWDOWN
//     digitalWrite(SHIFT_REGISTER_CLOCK, LOW);
//    SLOWDOWN

  }
}

void drawColumn(char character, size_t position) {
  position = position % WIDTH;
  // Offset of the char in the column
  size_t char_pos = position / 8;
  // Offset of the bit in the char
  size_t pos_char = position % 8;
  
  for (size_t row = 0; row < 8 ; row++) {
    unsigned char prev_char = allData[row][char_pos];
    if  (character & (1 << (row))) {
      allData[row][char_pos] = prev_char | (1 << (7 - (pos_char)));
    } else {
      allData[row][char_pos] = prev_char & (~(1 << (7 - (pos_char))));
    }
  }
}


OctafontRegular font;
//OctafontBold font;
unsigned char drawCharacter(char character, uint8_t position) {
//  if (character == ' '){
////    return 0;
//    drawColumn(0, position);
//    return 1;
//  }
  auto width = font.get_width(character);
  if (width == 255){
    character = '?';
    width = font.get_width(character);
  }
  for (auto i = 0; i < width;  i++) {
    auto octet = font.get_octet(character, i);
    drawColumn(octet, position + i);
  }
  return width;
}

void drawString(const char* string, uint8_t position) {
  size_t index;
  while (string[index] != 0){
    char thisChar = string[index];
    auto width = drawCharacter(thisChar, position);
    position += width;
 
    index+=1;
    char nextChar = string[index];

    if (!nextChar){
      continue;
    }
    if (PixelFont::get_undercut(font,thisChar,font,nextChar)){
      continue;
    }
    drawColumn(0, position);
    position++;
    
    
    
  }
}

void writeChars(const unsigned char* data, int size) {
  for (int index = size; index >= 0 ; index--) {
    writeChar(data[index]);
  }
}

hw_timer_t *timer = NULL;

void turnOff0() {
  digitalWrite(rowPins[0], HIGH);
}
void turnOff1() {
  digitalWrite(rowPins[1], HIGH);
}
void turnOff2() {
  digitalWrite(rowPins[2], HIGH);
}
void turnOff3() {
  digitalWrite(rowPins[3], HIGH);
}
void turnOff4() {
  digitalWrite(rowPins[4], HIGH);
}
void turnOff5() {
  digitalWrite(rowPins[5], HIGH);
}
void turnOff6() {
  digitalWrite(rowPins[6], HIGH);
}
void turnOff7() {
  digitalWrite(rowPins[7], HIGH);
  
}
void setTurnOffInterrupt(unsigned char row) {
 switch (row) {
  case 0:
   timerAttachInterrupt(timer, &turnOff0);
   break;
  case 1:
   timerAttachInterrupt(timer, &turnOff1);
   break;
  case 2:
   timerAttachInterrupt(timer, &turnOff2);
   break;
  case 3:
   timerAttachInterrupt(timer, &turnOff3);
   break;
  case 4:
   timerAttachInterrupt(timer, &turnOff4);
   break;
  case 5:
   timerAttachInterrupt(timer, &turnOff5);
   break;
  case 6:
   timerAttachInterrupt(timer, &turnOff6);
   break;
  case 7:
   timerAttachInterrupt(timer, &turnOff7);
   break;
 }
}

portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
void displayFrame() {
      serialStep();
    networkStep();
//  
  
  for (size_t row = 0; row < HEIGHT ; row++) {
    // Switch the previous row on
//    taskENTER_CRITICAL(&myMutex);
vTaskSuspendAll();
    
    
    timerRestart(timer);
    timerAlarm(timer, 300, false, 0);
    digitalWrite(rowPins[(row - 1) % HEIGHT], LOW);
    setTurnOffInterrupt((row - 1) % HEIGHT);
    
    // Push data
    writeChars(allData[row], (WIDTH / 8));



    while(timerRead(timer) < 350) {
     true;
//     digitalWrite(rowPins[(row - 1) % HEIGHT], HIGH);
    }
//    taskEXIT_CRITICAL(&myMutex);
 xTaskResumeAll();
//    timerDetachInterrupt(timer);




    auto low = ((row - 1) % HEIGHT) ? LOW : HIGH;
    auto high = row == 0 ? LOW : LOW;
    // Switch the new row on

    // Switch previous row off
//    digitalWrite(rowPins[(row - 1) % HEIGHT], HIGH);
//    SLOWDOWN

    // Disable output
    digitalWrite(OUTPUT_ENABLE, HIGH);
    SLOWDOWN



    // Move to output
    digitalWrite(REGISTER_CLOCK, HIGH);
//    delay(1);
    SLOWDOWN
    
    // Enable output
    digitalWrite(OUTPUT_ENABLE, LOW);
    SLOWDOWN
    digitalWrite(REGISTER_CLOCK, LOW);
    SLOWDOWN
    
  }
}

constexpr size_t MAX_CHARS_PER_PIXEL = 30
;

bool isStateSleeping(State& state) {
      if (state.sleep && state.sleepUntil < millis()) {
        state.sleep =  false;
      }
      return state.sleep;
}

void serialStep() {
  unsigned int charsRemaining = MAX_CHARS_PER_PIXEL;
  if (!isStateSleeping(serialState)){
  while (Serial.available() && charsRemaining > 0) {
    charsRemaining -= 1;
    unsigned char c = Serial.read();
    stepState(serialState, c, nullptr);
  }
  }
  if (serialState.sendingText != nullptr) {
  unsigned int sendChars = MAX_CHARS_PER_PIXEL;
  if (serialState.charactersLeft < sendChars ){
    sendChars = serialState.charactersLeft;
  }
  for (size_t i = 0; i < sendChars; i++) {
    Serial.write(serialState.sendingText[i]);
  }
  serialState.sendingText = &serialState.sendingText[sendChars];
  serialState.charactersLeft = serialState.charactersLeft - sendChars;
  if (serialState.charactersLeft <= 0) {
    serialState.sendingText = nullptr;
  }
 
  }
}

constexpr size_t MAX_CLIENTS = 20;
WiFiClient clients[MAX_CLIENTS];
State clientStates[MAX_CLIENTS];

void networkStep() {
  // Add new client if available
  for (size_t index = 0 ; index < MAX_CLIENTS ; index++) {
    auto client = clients[index];
    

    auto& state = clientStates[index];

    auto availableBytes = client.available();
    if (availableBytes && !isStateSleeping(state)) {

    auto readBytes  = 0;
    while (readBytes < availableBytes && readBytes < MAX_CHARS_PER_PIXEL) {
      readBytes++;
      auto data = client.read();
      stepState(clientStates[index], data, &client);
      //      client.write(data);
    }
    }

      if (state.sendingText != nullptr) {
  unsigned int sendChars = MAX_CHARS_PER_PIXEL;
  if (state.charactersLeft < sendChars ){
    sendChars = state.charactersLeft;
  }
  if (sendChars > 0) {
    char buffer[MAX_CHARS_PER_PIXEL + 1];
    memcpy(buffer, state.sendingText, sendChars);
    buffer[sendChars] = 0;
    client.print((char*)buffer);
    client.flush();
  }
//  for (size_t i = 0; i < sendChars; i++) {
//    
//    client.write(state.sendingText[i]);
//  }
  state.sendingText = &state.sendingText[sendChars];
  state.charactersLeft = state.charactersLeft - sendChars;
  if (state.charactersLeft <= 0) {
    state.sendingText = nullptr;
  }
 
  }

    //client.flush();
  }
}

const char* apSsid = "Strassenbahnanzeige";

WiFiServer server(23);
// TODO: Disabel when done with testing
constexpr bool SKIP_SCAN = true;

void setupWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("I am already connected");
    return;
  }

  WiFi.setHostname("strassenbahnanzeige");

  Serial.println("Ok, first I am opening my AP to make talking to me easier!");
//  WiFi.mode(WIFI_AP_STA);
WiFi.useStaticBuffers(true);
 WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  //  delay(1000);
  //   WiFi.softAPenableIPv6();
  IPAddress localIP(192, 168, 90, 1);
  IPAddress gateway(192, 168, 90, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(localIP,  gateway,   subnet);
  delay(2000);
  WiFi.softAP(apSsid, NULL);
  Serial.println("My IP on my own AP is:");
  Serial.println(WiFi.softAPIP());
  // Serial.println(WiFi.softAPIPv6());



  Serial.println("Scanning WiFi networks");

  // WiFi.scanNetworks will return the number of networks found
  int n = SKIP_SCAN ? 0 : WiFi.scanNetworks();

  Serial.println("I found ");
  Serial.print(n);
  Serial.println(" WiFi networks");

  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.print(")");
    Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
  }

  bool connected = false;
  const char* ssid = "Homeautomation";
//  #error Insert the WiFi password in the next line (and comment this line)
  const char* password = "AutomationIsMagic";
  for (int i = 0; i < n; ++i) {

    if (WiFi.SSID(i) == ssid) {
      Serial.print("Hey, I know ");
      Serial.print(ssid);
      Serial.println("!");
      Serial.print("I am trying to talk it.");

      WiFi.disconnect();
      WiFi.begin(ssid, password);
      size_t attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts++ <= 50) {
        Serial.print('.');
        delay(200);
      }
      Serial.println("");
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("That worked :D");
        Serial.print("Connected to ");
        Serial.println(ssid);
        break;
      }

      Serial.println("Seems like they dont want to talk to me");
    }
  }

  if (SKIP_SCAN) {
    Serial.println("Skipping wifi scan and connecting directly to speed up dev iterations");
    WiFi.begin(ssid, password);
    size_t attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts++ <= 50) {
      Serial.print('.');
      delay(100);
    }
    Serial.println("");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("I did not find any network that wants to communicate with me D:");
    // TODO: Open own AP now
  }

  Serial.println("Just FYI, my ipv4 is:");
  Serial.println(WiFi.localIP());

  // Wait a bit for no reason
  server.begin();
}


//HardwareSerial Serials(2);


void setupDisplay() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(OUTPUT_ENABLE, OUTPUT);
  digitalWrite(OUTPUT_ENABLE, HIGH);
  pinMode(DATA, OUTPUT);
  digitalWrite(DATA, LOW);
  pinMode(SHIFT_REGISTER_CLOCK, OUTPUT);
  digitalWrite(SHIFT_REGISTER_CLOCK, LOW);
  pinMode(REGISTER_CLOCK, OUTPUT);
  digitalWrite(REGISTER_CLOCK, LOW);

  pinMode(WAGEN_HALT, OUTPUT);
  digitalWrite(WAGEN_HALT, LOW);

  for (auto pin : rowPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  printGreeting(nullptr);
}

void setup()
{
  Serial.begin(115200);
  setupWifi();
  setupDisplay();

  timer = timerBegin(1000000);

  size_t offset = 0;
char message[101];  // max length you’ll need +1
snprintf(message, 100, "nc %s 23" , WiFi.localIP().toString().c_str());
  drawString(message, offset);
}



// Periodice wifi maintainance. Runs between each frame
void processWifi() {
  // Add new client if available
  WiFiClient client = server.accept();
  if (client) {
    int newClientIndex = -1;
    if (client.connected()) {


      for (size_t index = 0 ; index <= MAX_CLIENTS ; index++) {
        if (!clients[index]) {
          Serial.print("Connected to client ");
          Serial.print(client.remoteIP());
          Serial.print(" (");
          Serial.print(index);
          Serial.println(")");
          client.setNoDelay(true);
          {
            Serial.println("Connected server success");
            int keepAlive = 5000; // Milliseconds
            int  keepIdle = 5000; // The time (in seconds) the connection needs to remain idle before TCP starts sending keepalive probes, if the socket option SO_KEEPALIVE has been set on this socket.
            int  keepInterval = 5000; // The time (in seconds) between individual keepalive probes.
            int  keepCount = 1; // The maximum number of keepalive probes TCP should send before dropping the connection.
            if (client.setSocketOption(SOL_SOCKET, SO_KEEPALIVE, (void*)&keepCount, sizeof(keepCount)))
              Serial.println("TCP_KEEPALIVE failed");
            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)))
              Serial.println("TCP_KEEPALIVE failed");
            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle)))
              Serial.println("TCP_KEEPIDLE failed");
            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval)))
              Serial.println("TCP_KEEPINTVL failed");
            if (client.setSocketOption(IPPROTO_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount)))
              Serial.println("TCP_KEEPCNT failed");
          }
          clients[index].stop();
          clients[index] = client;
          newClientIndex = index;
          break;
        }
      }
      if (newClientIndex == -1) {
        Serial.print("Received connection from ");
        Serial.print(client.remoteIP());
        Serial.println(" but failed to add the new client because we cant handle more connections");
      }
    }
    if (newClientIndex == -1) {
      client.stop();
    }
  }

  unsigned int connectedClients = 0;
  //bool non = true;
  for (size_t index = 0 ; index < MAX_CLIENTS ; index++) {
    if (!clients[index]) {
      continue;
    }

    //    Serial.print(index);
    //    Serial.print(": ");
    //    Serial.print(clients[index].remoteIP());
    //    Serial.print(": ");
    //    Serial.print(clients[index].available());
    //    Serial.println(": ");
    //    non=false;

    if (!clients[index].connected()) {
      // Most disconnects are not logged here
      Serial.print(clients[index].remoteIP());
      Serial.println(" disconnected");
      clients[index].stop();
      clients[index] = WiFiClient();
      continue;
    }
    connectedClients++;
  }
  stats.currentConnections = connectedClients;
  if (connectedClients > stats.maxConnections){
  stats.maxConnections = connectedClients;}
  //  if(non){
  //    Serial.println("no clients ");}
}




void loop()
{

  // WiFI housekeeping between frames
  //  delay(1000);
  //  auto before = millis();
  
//  for (int i = 0 ; i < 8; i++){
//  for (int z = 0 ; z < 15; z++){
//    allData [i][z] = 0xff;
//    }
//    }

  
//  drawCharacter('%', 35);

//  offset = (offset + 1) % WIDTH;
//  drawColumn(0, offset - 1 + 120);


//  writeColumn(0x7c, 35);
//   writeColumn(0x12, 36);
//    writeColumn(0x11, 37);
//     writeColumn(0x12, 38);
//      writeColumn(0x7c, 39);
//for(size_t i = 0; i < 4 ; i++){
//  processWifi();
while (true){
  processWifi();
  displayFrame();
}
  //   auto after = millis();
  //
  //   auto wifiDuration = middle - before;
  //   auto displayDuration = after - middle;
  //   Serial.print("wifi rate: ");
  //   Serial.print(wifiDuration);
  //   Serial.print("display rate: ");
  //   Serial.println(displayDuration);

  //  writeAllData();
  //  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //  delay(1000);                       // wait for a second
  //  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  //  delay(1000);
  //  byte n = Serial.available();
  //  if (n != 0)
  //  {
  //    byte m = Serial.readBytesUntil('\n', myData, sizeof (myData) - 1);
  //    myData[m] = '\0';
  //    Serial.println(myData);
  //  }
  //  Serial.println("Hello");
}
