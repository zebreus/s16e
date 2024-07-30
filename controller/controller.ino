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

unsigned char allData[8][5] = {
  {0b10000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000},
  {0b10000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000},
  {0b10000000, 0b00100000, 0b00000000, 0b00000000, 0b00000000},
  {0b10000000, 0b00010000, 255, 255, 255},
  {0b10000000, 0b00001000, 0b00000000, 0b00000000, 0b00000000},
  {0b10000000, 0b00000100, 0b00000000, 0b00000000, 0b00000000},
  {0b10000000, 0b00000010, 0b00000000, 0b00000000, 0b00000000},
  {0b10000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000},
};

//#define ROW_PINS { ROW_0, ROW_1, ROW_2, ROW_3, ROW_4,  }

constexpr size_t rowPins[8] =  { ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7 };

constexpr unsigned char WIDTH = 120;
constexpr unsigned char HEIGHT = 8;

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


unsigned char nextX = 0;
unsigned char nextY = 0;

void serialStep() {
  if (!Serial.available()){
    return;
  }
  unsigned char c = Serial.read();
  switch (state) {
    case STATE_IDLE: {
      state = c == 'P' ? STATE_P : STATE_IDLE;
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
      if (number >= 10){
        state = STATE_IDLE;
      } else {
        nextX = number;
        state = STATE_PX_X1;
      }
      
      }
      break;
    case STATE_PX_X1: {
      Serial.print("test1");
      if (c == ' ') {
        state = STATE_PX_X_SPACE;
        break;
      }
      unsigned char number = c - 48;
      if (number >= 10){
        state = STATE_IDLE;
      } else {
        nextX += number * 10;
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
      if (number >= 10){
        state = STATE_IDLE;
      } else {
        nextX += number * 100;
        state = STATE_PX_X3;
      }
      
      }
      break;
    case STATE_PX_X3: {
      Serial.print("test2");
      state = c == ' ' ? STATE_PX_X_SPACE : STATE_IDLE;
      }
      break;
    case STATE_PX_X_SPACE: {
      unsigned char number = c - 48;
      if (number >= 10){
        state = STATE_IDLE;
      } else {
        nextX = number;
        state = STATE_PX_Y;
      }
      
      }
      break;
    case STATE_PX_Y: {
      state = c == ' ' ? STATE_PX_Y_SPACE : STATE_IDLE;
      }
      break;
    case STATE_PX_Y_SPACE: {
        auto oldValue = allData[nextY][(nextX / 8)]; 
        unsigned char value = (c == 'f') ? 0 : 1;
        Serial.print("test3");
        if  (c == 'f') {
          
          allData[nextY][(nextX / 8)] = oldValue | (1 << (value % 8));
        }else {
          allData[nextY][(nextX / 8)] = oldValue & (~(1 << (value % 8)));
        }
        state = STATE_IDLE;
      }
      break;
    default:
      state = STATE_IDLE;
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

   for (auto pin : rowPins) {
     pinMode(pin, OUTPUT);
     digitalWrite(pin, HIGH);
   }
   Serial.begin(9600);
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
  for (size_t row = 0; row < 8 ; row++){
    writeChars(allData[row], 5);

    // Disable output
    digitalWrite(OUTPUT_ENABLE, HIGH);

    auto low = ((row - 1) % 8) ? LOW : HIGH;
    auto high = row == 0 ? LOW : LOW;
    // Switch the new row on
    digitalWrite(rowPins[(row - 1) % 8], HIGH);
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
//  digitalWrite(OUTPUT_ENABLE, HIGH);
//  digitalWrite(REGISTER_CLOCK, LOW);
//  allData[0][1] = 0b10000000;
//  allData[0][3] = 0b00010100;
//
//  writeChars(allData[0], 5);
//
//  delay(SPEED);
//  digitalWrite(REGISTER_CLOCK, HIGH);
//  delay(SPEED);
//  digitalWrite(REGISTER_CLOCK, LOW);
//  delay(SPEED);
//  
//  digitalWrite(OUTPUT_ENABLE, LOW);
writeAllData();
//  delay(1000);
}
