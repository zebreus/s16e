// Output enable low active
#define OUTPUT_ENABLE 10
// Serial data
#define DATA 11
// Shift register clock
#define SHIFT_REGISTER_CLOCK 12
// Register clock
#define REGISTER_CLOCK 13



#define ROW_0 2
#define ROW_1 3
#define ROW_2 4
#define ROW_3 5
#define ROW_4 6
#define ROW_5 7
#define ROW_6 8
#define ROW_7 9

#define WAGEN_HALT A3

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

unsigned char state = 0;

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

unsigned char nextX = 0;
unsigned char nextY = 0;

size_t spIndex = 0;
constexpr size_t MAX_CHARS_PER_PIXEL = 5;
void serialStep() {
  unsigned int charsRemaining = MAX_CHARS_PER_PIXEL;
  while (Serial.available() && charsRemaining > 0) {
    charsRemaining -= 1;

    unsigned char c = Serial.read();
    switch (state) {
      case STATE_IDLE: {
          state = c == 'P' ? STATE_P : c == 'S' ? STATE_S : c == 'W' ? STATE_W : STATE_IDLE;
        }
        break;
      case STATE_P: {
          state = c == 'B' ? STATE_PB : c == 'X' ? STATE_PX : STATE_IDLE;
        }
        break;

      // PB: Binary
      case STATE_PB: {
          nextX = c - '0';
          state = nextX < WIDTH ? STATE_X : STATE_IDLE;
        }
        break;
      case STATE_X: {
          nextY = c - '0';
          state = nextY < HEIGHT ? STATE_Y : STATE_IDLE;
        }
        break;
      case STATE_Y: {
          unsigned char value = (c == 0 || c == '0') ? 0 : 1;
          allData[nextY][nextX] = value;
          state = STATE_IDLE;
        }
        break;

      // PX: Ascii
      case STATE_PX: {
          state = c == ' ' ? STATE_PX_SPACE : STATE_IDLE;
        }
        break;
      case STATE_PX_SPACE: {
          unsigned char number = c - 48;
          if (number >= 10) {
            state = STATE_IDLE;
          } else {
            nextX = number;
            state = STATE_PX_X1;
          }

        }
        break;
      case STATE_PX_X1: {
          if (c == ' ') {
            state = STATE_PX_X_SPACE;
            break;
          }
          unsigned char number = c - 48;
          if (number >= 10) {
            state = STATE_IDLE;
          } else {
            nextX = number + nextX * 10;
            state = STATE_PX_X2;
          }

        }
        break;
      case STATE_PX_X2: {
          if (c == ' ') {
            state = STATE_PX_X_SPACE;
            break;
          }
          unsigned char number = c - 48;
          if (number >= 10) {
            state = STATE_IDLE;
          } else {
            nextX = number + nextX * 10;
            state = STATE_PX_X3;
          }

        }
        break;
      case STATE_PX_X3: {
          state = c == ' ' ? STATE_PX_X_SPACE : STATE_IDLE;
        }
        break;
      case STATE_PX_X_SPACE: {
          unsigned char number = c - 48;
          if (number >= 10) {
            state = STATE_IDLE;
          } else {
            nextY = number;
            state = STATE_PX_Y;
          }

        }
        break;
      case STATE_PX_Y: {
          state = c == ' ' ? STATE_PX_Y_SPACE : STATE_IDLE;
        }
        break;
      case STATE_PX_Y_SPACE: {
          unsigned char oldValue = allData[nextY][(nextX / 8)];
          unsigned char value = (c == 'f') ? 1 : 0;
          if  (c == 'f') {
//            Serial.println("Turning it on");
//            Serial.print("Y");
//            Serial.println(nextY);
//            Serial.print("X");
//            Serial.println(nextX / 8);
//                        Serial.print("X");
//            Serial.println((nextX % 8));
//                                  Serial.print("oldval");
//            Serial.println(oldValue, BIN);
//             Serial.print("newval");
//            Serial.println(oldValue | (1 << (7-(nextX % 8))), BIN);
            allData[nextY][(nextX / 8)] = oldValue | (1 << (7-(nextX % 8)));
          } else {
            allData[nextY][(nextX / 8)] = oldValue & (~(1 << (7-(nextX % 8))));
          }
          state = STATE_IDLE;
        }
        break;

      // SP: Full update binary
        case STATE_S: {
          spIndex = 0;
          state = c == 'P' ? STATE_SP : STATE_IDLE;
        }
        break;
        case STATE_SP: {
          ((unsigned char*)allData)[spIndex] = c;
 
          if (spIndex == ((HEIGHT*(WIDTH/8))-1)) {
            state = STATE_IDLE;
          } else {
            state = STATE_SP;
            spIndex += 1;
          }
        }
        break;

      // WH: Wagen hält (WH1 = on; WH0 = off)
      case STATE_W: {
        state = c == 'H' ? STATE_WH : STATE_IDLE;
      }
      break;
      case STATE_WH: {
        digitalWrite(WAGEN_HALT, (c == '1') ? HIGH : LOW);
        state = STATE_IDLE;
      }
      break;
      default:
        state = STATE_IDLE;
    }
  }

}

// the setup function runs once when you press reset or power the board
void setup() {
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
  Serial.begin(115200);
  Serial.println("Welcome to");
Serial.println("   ____  _  __      ");  
Serial.println("/ ___|/ |/ /_   ___ ");
Serial.println("\\___ \\| | '_ \\ / _ \\");
Serial.println(" ___) | | (_) |  __/");
Serial.println("|____/|_|\\___/ \\___|");
Serial.println("");
Serial.println("This display supports some pixelflut commands and some own commands. The resolution is 128x8. ");
Serial.println("Here is a overview of the supported commands:");
Serial.println("PX XXX Y f: Activate the pixel at the supplied X and Y coordinates. X and Y are ascii numbers");
Serial.println("PX XXX Y 0: Deactivate the pixel at the supplied X and Y coordinates. X and Y are ascii numbers");
Serial.println("PBXY1: Activate the pixel at the supplied X and Y coordinates. X and Y are one byte binary numbers");
Serial.println("PBXY0: Deactivate the pixel at the supplied X and Y coordinates. X and Y are one byte binary numbers");
Serial.println("WH1: Activate the 'Wagen halt' indicator");
Serial.println("WH0: Deactivate the 'Wagen halt' indicator");
Serial.println("SP: Update the whole image. Send SP and then a bitstream for the whole image");
Serial.println("");
Serial.println("Thank you for displaying with S16e");
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
  for (size_t index = 0; index < 8 ; index++) {
    serialStep();

    digitalWrite(SHIFT_REGISTER_CLOCK, LOW);
    digitalWrite(DATA, data & (1 << index));
    digitalWrite(SHIFT_REGISTER_CLOCK, HIGH);
  }
}

void writeChars(unsigned char* data, int size) {
  allData[0][0] = state;
  for (int index = size; index >= 0 ; index--) {
    writeChar(data[index]);
  }
}

void writeAllData() {
  for (size_t row = 0; row < HEIGHT ; row++) {
    writeChars(allData[row], (WIDTH/8));

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

// the loop function runs over and over again forever
void loop() {
  writeAllData();
}
