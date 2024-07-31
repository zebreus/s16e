#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

// Output enable low active
#define OUTPUT_ENABLE 0
// Serial data
#define DATA 1
// Shift register clock
#define SHIFT_REGISTER_CLOCK 2
// Register clock
#define REGISTER_CLOCK 3



#define ROW_0 5
#define ROW_1 6
#define ROW_2 7
#define ROW_3 8
#define ROW_4 9
#define ROW_5 10
#define ROW_6 20
#define ROW_7 21

#define WAGEN_HALT 4

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

unsigned char allData[8][15] = {
  {0b00000000, 0b01111110, 0b00000000, 0b00001100, 0b00000000, 0b00000000, 0b01111100, 0b00000000, 0b01111100, 0b00000000, 0b00011110, 0b00000000, 0b01111110, 0b00000000, 0b00000000},
  {0b00000000, 0b10000010, 0b00000000, 0b00010010, 0b00000000, 0b00000000, 0b10000010, 0b00000000, 0b10000010, 0b00000000, 0b01100010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
  {0b00000000, 0b10000000, 0b00000000, 0b00100010, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b10000000, 0b00000000, 0b11000010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
  {0b00000000, 0b01111100, 0b00000000, 0b00100000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b01111100, 0b00000000, 0b00000010, 0b00000000, 0b01111100, 0b00000000, 0b00000000},
  {0b00000000, 0b00000010, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b00000010, 0b00000000, 0b00000010, 0b00000000, 0b00000010, 0b00000000, 0b00000000},
  {0b00000000, 0b10000010, 0b00000000, 0b00010010, 0b00000000, 0b00000000, 0b10000000, 0b00000000, 0b10000010, 0b00000000, 0b00000010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
  {0b00000000, 0b10000010, 0b00000000, 0b00001100, 0b00000000, 0b00000000, 0b10000010, 0b00000000, 0b10000010, 0b00000000, 0b00000010, 0b00000000, 0b10000010, 0b00000000, 0b00000000},
  {0b00000000, 0b01111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01111100, 0b00000000, 0b01111100, 0b00000000, 0b00011110, 0b00000000, 0b01111100, 0b00000000, 0b00000000}
};


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
};

State serialState;
constexpr size_t MAX_CHARS_PER_PIXEL = 15 * 8;


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

void printHelp(WiFiClient* client) {
  clientPrintln(client, "This display supports the common pixelflut commands and some own commands:");
  clientPrintln(client, "Pixelflut commands:");
  clientPrintln(client, "  HELP: Print this help message");
  clientPrintln(client, "  SIZE: Returns the size of the visible canvas in pixel as SIZE <w> <h>");
  clientPrintln(client, "  PX <x> <y> Return the current color of a pixel as PX <x> <y> <rrggbb>");
  clientPrintln(client, "  PX <x> <y> <rrggbb(aa)>: Draw a single pixel at position (x, y) with the specified hex color code. ffffff is on; everything else is off.");
  clientPrintln(client, "Light up the 'Wagen hält' indicator:");
  clientPrintln(client, "  WHf: Activate the 'Wagen halt' indicator");
  clientPrintln(client, "  WH0: Deactivate the 'Wagen halt' indicator");
  clientPrintln(client, "Shorthands for PX:");
  clientPrintln(client, "  PX <x> <y> 0: Set the pixel at position (x, y) to black");
  clientPrintln(client, "  PX <x> <y> f: Set the pixel at position (x, y) to white");
  clientPrintln(client, "Binary commands (No newline required):");
  clientPrintln(client, "  PB<x><y><v>: Set the pixel at (x, y) to brightness <v>. <x>, <y> and <v> are one byte binary numbers.");
  clientPrintln(client, "  SP: Update the whole image. Send SP and then a bitstream for the whole image");


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
        state.state = c == 'P' ? STATE_P : c == 'S' ? STATE_S : c == 'W' ? STATE_W : c == 'H' ? STATE_H : STATE_IDLE;
      }
      break;
    case STATE_P: {
        state.state = c == 'B' ? STATE_PB : c == 'X' ? STATE_PX : STATE_IDLE;
      }
      break;

    // PB: Binary
    case STATE_PB: {
        state.nextX = c - '0';
        state.state = state.nextX < WIDTH ? STATE_X : STATE_IDLE;
      }
      break;
    case STATE_X: {
        state.nextY = c - '0';
        state.state = state.nextY < HEIGHT ? STATE_Y : STATE_IDLE;
      }
      break;
    case STATE_Y: {
        unsigned char value = (c == 0 || c == '0') ? 0 : 1;
        allData[state.nextY][state.nextX] = value;
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
            if (state.asciiColorIndex == 1 && (state.red == 0xf0u)) {
              display = true;
              state.red = 0xffu;
              state.green = 0xffu;
              state.blue = 0xffu;
              state.alpha = 0xffu;
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
        state.state = c == 'P' ? STATE_SP : c == 'I' ? STATE_SI :  c == 'T' ? STATE_ST : STATE_IDLE;
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
          printHelp(client);
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

    default:
      state.state = STATE_IDLE;
  }


}


#define SPEED 0

// Set the next bit a single bit
void writeData(bool data) {
  digitalWrite(SHIFT_REGISTER_CLOCK, LOW);
  digitalWrite(DATA, data);
  digitalWrite(SHIFT_REGISTER_CLOCK, HIGH);
}

// Write 8 bits
void writeChar(unsigned char data) {
  serialStep();
  networkStep();
  for (size_t index = 0; index < 8 ; index++) {


    digitalWrite(SHIFT_REGISTER_CLOCK, LOW);
    digitalWrite(DATA, data & (1 << index));
    digitalWrite(SHIFT_REGISTER_CLOCK, HIGH);
  }
}

void writeChars(unsigned char* data, int size) {
  for (int index = size; index >= 0 ; index--) {
    writeChar(data[index]);
  }
}

void displayFrame() {
  for (size_t row = 0; row < HEIGHT ; row++) {
    writeChars(allData[row], (WIDTH / 8));

    // Disable output
    digitalWrite(OUTPUT_ENABLE, HIGH);

    auto low = ((row - 1) % HEIGHT) ? LOW : HIGH;
    auto high = row == 0 ? LOW : LOW;
    // Switch the new row on
    digitalWrite(rowPins[(row - 1) % HEIGHT], HIGH);
    digitalWrite(rowPins[row], LOW);

    // Move to output
    digitalWrite(REGISTER_CLOCK, HIGH);
    digitalWrite(REGISTER_CLOCK, LOW);

    // Enable output
    digitalWrite(OUTPUT_ENABLE, LOW);
  }
}

void serialStep() {
  unsigned int charsRemaining = MAX_CHARS_PER_PIXEL;
  while (Serial.available() && charsRemaining > 0) {
    charsRemaining -= 1;
    unsigned char c = Serial.read();
    stepState(serialState, c, nullptr);
  }
}

constexpr size_t MAX_CLIENTS = 20;
WiFiClient clients[MAX_CLIENTS];
State clientStates[MAX_CLIENTS];

void networkStep() {
  // Add new client if available
  for (size_t index = 0 ; index < MAX_CLIENTS ; index++) {
    auto client = clients[index];

    auto availableBytes = client.available();
    if (!availableBytes) {
      continue;
    }
    auto readBytes  = 0;
    while (readBytes < availableBytes && readBytes < MAX_CHARS_PER_PIXEL) {
      readBytes++;
      auto data = client.read();
      stepState(clientStates[index], data, &client);
      //      client.write(data);
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
  //    Serial.println(WiFi.softAPIPv6());



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
  const char* ssid = "Haselnussnetz";
  const char* password = "policelinedonotcross";
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
  digitalWrite(WAGEN_HALT, HIGH);

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
  }
  //  if(non){
  //    Serial.println("no clients ");}
}




void loop()
{

  // WiFI housekeeping between frames
  //  delay(1000);
  //  auto before = millis();
  processWifi();
  //   // Display frame
  //   auto middle = millis();
  displayFrame();
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
