#include "setup.hpp"
#include "../config.hpp"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "fonts/octafont-regular.hpp"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include <cstring>
#include <stdio.h>
#include <string.h>

unsigned char allData[8][15];

// This gets buggy sometimes
void rotate(unsigned char x, unsigned char y) {
  // TODO: Make this more efficient
  for (int ii = 0; ii < (x % WIDTH); ii++) {
    for (int row = 0; row < HEIGHT; row++) {
      //      char first = allData[col] >> 1 | allData[(col+1) % WIDTH] << 7;
      char last = (allData[row][0] >> 1) | (allData[row][(WIDTH / 8) - 1] << 7);
      for (int col = (WIDTH / 8) - 1; col > 0; col--) {
        allData[row][col] =
            (allData[row][col] >> 1) | (allData[row][(col - 1)] << 7);
      }
      allData[row][0] = last;
    }
  }

  // TODO: Make this more efficient
  for (size_t ii = 0; ii < (y % HEIGHT); ii++) {
    char tmpRow[WIDTH / 8];
    memcpy(tmpRow, allData[(1) % HEIGHT], WIDTH / 8);
    for (size_t i = 1; i < HEIGHT; i++) {
      memcpy(allData[i], allData[(i + 1) % HEIGHT], WIDTH / 8);
    }
    memcpy(allData[0], tmpRow, WIDTH / 8);
  }
}

void drawColumn(char character, size_t position) {
  position = position % WIDTH;
  // Offset of the char in the column
  size_t char_pos = position / 8;
  // Offset of the bit in the char
  size_t pos_char = position % 8;

  for (size_t row = 0; row < 8; row++) {
    unsigned char prev_char = allData[row][char_pos];
    if (character & (1 << (row))) {
      allData[row][char_pos] = prev_char | (1 << (7 - (pos_char)));
    } else {
      allData[row][char_pos] = prev_char & (~(1 << (7 - (pos_char))));
    }
  }
}

// Write 8 bits
void writeChar(unsigned char data) {
  for (size_t index = 0; index < 8; index++) {
    gpio_set_level(SHIFT_REGISTER_CLOCK, 0);
    gpio_set_level(DATA, (data & (1 << index)) ? 1 : 0);
    gpio_set_level(SHIFT_REGISTER_CLOCK, 1);
    //    gpio_set_level(DATA, (data & (1 << index)) ? 0 : 1);
    //    SLOWDOWN
    //     gpio_set_level(SHIFT_REGISTER_CLOCK, 0);
    //    SLOWDOWN
  }
}

OctafontRegular font;
// OctafontBold font;
unsigned char drawCharacter(char character, uint8_t position) {
  // return 1;
  auto width = font.get_width(character);
  if (width == 255) {
    character = '?';
    width = font.get_width(character);
  }
  for (auto i = 0; i < width; i++) {
    auto octet = font.get_octet(character, i);
    drawColumn(octet, position + i);
  }
  return width;
}

void drawString(const char *string, uint8_t position) {
  size_t index = 0;
  while (string[index] != 0) {
    char thisChar = string[index];
    auto width = drawCharacter(thisChar, position);
    position += width;

    index += 1;
    char nextChar = string[index];

    if (!nextChar) {
      continue;
    }
    // if (PixelFont::get_undercut(font, thisChar, font, nextChar)) {
    //   continue;
    // }
    drawColumn(0, position);
    position++;
  }
}

void writeChars(const unsigned char *data, int size) {
  for (int index = size; index >= 0; index--) {
    writeChar(data[index]);
  }
}

unsigned long IRAM_ATTR micros() {
  return (unsigned long)(esp_timer_get_time());
}

void IRAM_ATTR delayMicroseconds(uint32_t us) {
  uint32_t m = micros();
  if (us) {
    uint32_t e = (m + us);
    if (m > e) { // overflow
      while (micros() > e) {
        __asm__ __volatile__("nop");
      }
    }
    while (micros() < e) {
      __asm__ __volatile__("nop");
    }
  }
}

// hw_timer_t *timer = NULL;

// void turnOff0() {
//   gpio_set_level(ROW_PINS[0], 1);
// }
// void turnOff1() {
//   gpio_set_level(ROW_PINS[1], 1);
// }
// void turnOff2() {
//   gpio_set_level(ROW_PINS[2], 1);
// }
// void turnOff3() {
//   gpio_set_level(ROW_PINS[3], 1);
// }
// void turnOff4() {
//   gpio_set_level(ROW_PINS[4], 1);
// }
// void turnOff5() {
//   gpio_set_level(ROW_PINS[5], 1);
// }
// void turnOff6() {
//   gpio_set_level(ROW_PINS[6], 1);
// }
// void turnOff7() {
//   gpio_set_level(ROW_PINS[7], 1);

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

// portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
void displayFrame() {
  //   serialStep();
  // networkStep();
  //

  for (size_t row = 0; row < HEIGHT; row++) {
    // Switch the previous row on
    //    taskENTER_CRITICAL(&myMutex);
    vTaskSuspendAll();

    // timerRestart(timer);
    // timerAlarm(timer, 300, false, 0);
    gpio_set_level(ROW_PINS[(row - 1) % HEIGHT], 0);
    // setTurnOffInterrupt((row - 1) % HEIGHT);

    // Push data
    writeChars(allData[row], (WIDTH / 8));
    delayMicroseconds(1000);
    gpio_set_level(ROW_PINS[(row - 1) % HEIGHT], 1);

    // while(timerRead(timer) < 350) {
    //  true;
    // }

    xTaskResumeAll();
    //    timerDetachInterrupt(timer);

    // Switch the new row on

    // Switch previous row off
    //    gpio_set_level(ROW_PINS[(row - 1) % HEIGHT], 1);
    //    SLOWDOWN

    // Disable output
    gpio_set_level(OUTPUT_ENABLE, 1);

    // Move to output
    gpio_set_level(REGISTER_CLOCK, 1);
    //    delay(1);

    // Enable output
    gpio_set_level(OUTPUT_ENABLE, 0);
    gpio_set_level(REGISTER_CLOCK, 0);
  }
}

void setupDisplay() {
  gpio_config_t outputConfig = {};
  // disable interrupt
  outputConfig.intr_type = GPIO_INTR_DISABLE;
  // set as output mode
  outputConfig.mode = GPIO_MODE_OUTPUT;
  // bit mask of the pins that you want to set,e.g.GPIO18/19
  outputConfig.pin_bit_mask |= (1ULL << OUTPUT_ENABLE);
  outputConfig.pin_bit_mask |= (1ULL << DATA);
  outputConfig.pin_bit_mask |= (1ULL << SHIFT_REGISTER_CLOCK);
  outputConfig.pin_bit_mask |= (1ULL << REGISTER_CLOCK);
  outputConfig.pin_bit_mask |= (1ULL << WAGEN_HALT);
  for (auto pin : ROW_PINS) {
    outputConfig.pin_bit_mask |= (1ULL << pin);
  }
  // disable pull-down mode
  outputConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  // disable pull-up mode
  outputConfig.pull_up_en = GPIO_PULLUP_DISABLE;

  gpio_config(&outputConfig);
  gpio_set_level(OUTPUT_ENABLE, 1);
  gpio_set_level(DATA, 0);
  gpio_set_level(SHIFT_REGISTER_CLOCK, 0);
  gpio_set_level(REGISTER_CLOCK, 0);
  gpio_set_level(WAGEN_HALT, 0);
  for (auto pin : ROW_PINS) {
    gpio_set_level(pin, 1);
  }
}