#include "Display.hpp"
#include "../config.hpp"
#include "../stats.hpp"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "driver/timer_types_legacy.h"
#include "esp_log.h"
#include "fonts/octafont-regular.hpp"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "hal/timer_types.h"
#include <cstring>
#include <stdio.h>
#include <string.h>

void writeChar(unsigned char data);
void writeChars(const unsigned char *data, int size);

void enableOutput() { gpio_set_level(OUTPUT_ENABLE, 0); }
void disableOutput() { gpio_set_level(OUTPUT_ENABLE, 1); }
void shiftRowToOutput() { gpio_set_level(REGISTER_CLOCK, 1); }
void resetShiftRowToOutput() { gpio_set_level(REGISTER_CLOCK, 0); }
void enableRow(int row) {
  gpio_set_level(ROW_PINS[(row + HEIGHT - 1) % HEIGHT], 0);
}
void disableRow(int row) {
  gpio_set_level(ROW_PINS[(row + HEIGHT - 1) % HEIGHT], 1);
}

void writeRow(unsigned char (&rowData)[WIDTH], unsigned char bit) {
  // unsigned char srgbRow[WIDTH];
  // for (unsigned char i = 0; i < WIDTH; i++) {
  //   srgbRow[i] = srgbToLinear(rowData[i]);
  // }

  unsigned char bitstream[WIDTH / 8];
  for (unsigned char column = 0; column < WIDTH; column += 8) {
    bitstream[column / 8] =
        (rowData[column + 0] & (1 << bit) ? 0b10000000 : 0) |
        (rowData[column + 1] & (1 << bit) ? 0b01000000 : 0) |
        (rowData[column + 2] & (1 << bit) ? 0b00100000 : 0) |
        (rowData[column + 3] & (1 << bit) ? 0b00010000 : 0) |
        (rowData[column + 4] & (1 << bit) ? 0b00001000 : 0) |
        (rowData[column + 5] & (1 << bit) ? 0b00000100 : 0) |
        (rowData[column + 6] & (1 << bit) ? 0b00000010 : 0) |
        (rowData[column + 7] & (1 << bit) ? 0b00000001 : 0);
  }
  writeChars(bitstream, (WIDTH / 8));
}

volatile bool triggered;
static bool IRAM_ATTR timerCallback(void *args) {
  triggered = true;
  disableOutput();
  return true;
}

void initializeTimer() {
  static bool initializedTimer = false;
  if (!initializedTimer) {

    timer_config_t timerConfig{
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = 2,
    };

    triggered = false;
    timer_init(TIMER_GROUP_1, TIMER_0, &timerConfig);
    // timer_isr_register(TIMER_GROUP_1, TIMER_0, &timerCallback2, NULL, 0,
    // nullptr);
    timer_isr_callback_add(TIMER_GROUP_1, TIMER_0, timerCallback, NULL, 0);
    timer_group_intr_enable(TIMER_GROUP_1, TIMER_INTR_T0);
    timer_enable_intr(TIMER_GROUP_1, TIMER_0);
    initializedTimer = true;
  }
}

void startTimer(uint64_t duration) {
  timer_pause(TIMER_GROUP_1, TIMER_0);
  triggered = false;
  timer_set_alarm_value(TIMER_GROUP_1, TIMER_0, duration);
  timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0);
  timer_start(TIMER_GROUP_1, TIMER_0);
  // timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0);
}

void waitForTimer() {
  while (!triggered) {
    __asm__ __volatile__("nop");

    // ESP_LOGE("toast", "Not triggered");
  }
  // ESP_LOGI("toast", "Triggered");
  timer_pause(TIMER_GROUP_1, TIMER_0);
}

void Display::show() {
  //   serialStep();
  // networkStep();
  constexpr size_t BRIGHTNESS_BITS = 6;

  initializeTimer();

  vTaskSuspendAll();

  // Why does this work???
  for (size_t row = 0; row < HEIGHT; row++) {
    for (auto i = 0; i < BRIGHTNESS_BITS; i++) {
      // Switch the previous row on
      //    taskENTER_CRITICAL(&myMutex);

      // setTurnOffInterrupt((row - 1) % HEIGHT);
      // Disable output
      disableOutput();
      // Move to output
      shiftRowToOutput();
      // Enable output
      enableOutput();
      // Reset register clock
      resetShiftRowToOutput();

      if (i == 0) {
        disableRow(row - 2);
        enableRow(row - 1);
      } else {
        disableRow(row - 1);
        enableRow(row);
      }
      int previousI = (i == 0 ? BRIGHTNESS_BITS - 1 : i - 1);
      size_t timingMultiplier = 1;
      for (auto b = 0; b < previousI; b++) {
        timingMultiplier *= 2.8;
      }
      startTimer(200 * timingMultiplier);
      // Why 9
      writeRow(frameData[row], i + (8 - BRIGHTNESS_BITS));
      waitForTimer();
    }
  }
  disableRow(6);
  disableRow(7);
  disableRow(1);

  xTaskResumeAll();
}

// void Display::show() {
//   //   serialStep();
//   // networkStep();

//   vTaskSuspendAll();
//   for (size_t row = 0; row < HEIGHT; row++) {
//     // Switch the previous row on
//     //    taskENTER_CRITICAL(&myMutex);
//     unsigned char srgbRow[WIDTH];
//     for (unsigned char i = 0; i < WIDTH; i++) {
//       srgbRow[i] = srgbToLinear(frameData[row][i]);
//     }

//     // timerRestart(timer);
//     // timerAlarm(timer, 300, false, 0);
//     gpio_set_level(ROW_PINS[(row - 1) % HEIGHT], 0);
//     // setTurnOffInterrupt((row - 1) % HEIGHT);
//     for (auto subFrame = 0; subFrame < SUBFRAMES; subFrame++) {
//       unsigned char rowData[WIDTH / 8];
//       unsigned char threshold = (int)(255.0 * (subFrame + 0.5) / SUBFRAMES);
//       for (unsigned char column = 0; column < WIDTH; column += 8) {
//         rowData[column / 8] =
//             (srgbRow[column + 0] >= threshold ? 0b10000000 : 0) |
//             (srgbRow[column + 1] >= threshold ? 0b01000000 : 0) |
//             (srgbRow[column + 2] >= threshold ? 0b00100000 : 0) |
//             (srgbRow[column + 3] >= threshold ? 0b00010000 : 0) |
//             (srgbRow[column + 4] >= threshold ? 0b00001000 : 0) |
//             (srgbRow[column + 5] >= threshold ? 0b00000100 : 0) |
//             (srgbRow[column + 6] >= threshold ? 0b00000010 : 0) |
//             (srgbRow[column + 7] >= threshold ? 0b00000001 : 0);
//       }

//       writeChars(rowData, (WIDTH / 8));
//       // Disable output
//       gpio_set_level(OUTPUT_ENABLE, 1);
//       // Move to output
//       gpio_set_level(REGISTER_CLOCK, 1);
//       // Enable output
//       gpio_set_level(OUTPUT_ENABLE, 0);
//       // Reset register clock
//       gpio_set_level(REGISTER_CLOCK, 0);
//     }

//     // Set to zero
//     unsigned char rowData[15] = {0};
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     writeChars(rowData, (WIDTH / 8));
//     gpio_set_level(OUTPUT_ENABLE, 1);
//     gpio_set_level(REGISTER_CLOCK, 1);
//     gpio_set_level(OUTPUT_ENABLE, 0);
//     gpio_set_level(REGISTER_CLOCK, 0);

//     // Push data
//     // delayMicroseconds(1000);
//     gpio_set_level(ROW_PINS[(row - 1) % HEIGHT], 1);
//   }
//   xTaskResumeAll();
// }

void Display::setPixel(unsigned char x, unsigned char y,
                       unsigned char brightness, unsigned char transparency) {
  frameData[y][x] = brightness;
  stats.pixelsDrawn += 1;
}
unsigned char Display::getPixel(unsigned char x, unsigned char y) {
  return frameData[y][x];
}

void Display::drawRow(unsigned char (&rowData)[WIDTH], unsigned char row) {
  memcpy(frameData[row % HEIGHT], rowData, WIDTH);
}

void Display::drawColumn(unsigned char (&columnData)[HEIGHT],
                         unsigned char column) {
  column = column % WIDTH;

  for (size_t row = 0; row < HEIGHT; row++) {
    frameData[row][column] = columnData[row];
  }
}

// OctafontBold font;
unsigned char Display::drawCharacter(char character, unsigned char x,
                                     unsigned char y) {
  static OctafontRegular font;
  // return 1;
  auto width = font.get_width(character);
  if (width == 255) {
    character = '?';
    width = font.get_width(character);
  }
  for (auto i = 0; i < width; i++) {
    auto octet = font.get_octet(character, i);
    for (auto columnIndex = 0; columnIndex < 8; columnIndex++) {
      frameData[(y + columnIndex) % HEIGHT][(x + i) % WIDTH] =
          (octet & (1 << columnIndex)) == 0 ? 0 : 255;
    }
  }
  return width;
}

void Display::drawString(const char *string, unsigned char x, unsigned char y) {
  size_t index = 0;
  while (string[index] != 0) {
    char thisChar = string[index];
    auto width = drawCharacter(thisChar, x, y);
    x += width;

    index += 1;
    char nextChar = string[index];

    if (!nextChar) {
      continue;
    }
    // if (PixelFont::get_undercut(font, thisChar, font, nextChar)) {
    //   continue;
    // }
    unsigned char emptyColumn[HEIGHT] = {0};
    drawColumn(emptyColumn, x);
    x++;
  }
}

Display display;

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

Display::Display() {
  for (int x = 0; x < 120; x++) {
    frameData[2 + ((x / 120) * 2)][x % 120] =
        ((unsigned char)((x * 2.5) / 1)) % 256;
  }
  //  setPixel(x % 120, 2 + ((x // 120) * 2), ((x * 2.5) // 1) % 255 , 255)
}

void writeChars(const unsigned char *data, int size) {
  for (int index = size; index >= 0; index--) {
    writeChar(data[index]);
  }
}

// Write 8 bits
void writeChar(unsigned char data) {
  for (size_t index = 0; index < 8; index++) {
    gpio_set_level(SHIFT_REGISTER_CLOCK, 0);
    gpio_set_level(DATA, (data & (1 << index)) ? 1 : 0);
    gpio_set_level(SHIFT_REGISTER_CLOCK, 1);
  }
}

// This gets buggy sometimes
void Display::rotate(unsigned char x, unsigned char y) {
  static unsigned char temp[HEIGHT][WIDTH];
  // Shift row data while copying into temp buffer
  for (int row = 0; row < HEIGHT; row++) {
    memcpy(temp[row], frameData[row] + (x % WIDTH), WIDTH - (x % WIDTH));
    memcpy(temp[row] + WIDTH - (x % WIDTH), frameData[row], (x % WIDTH));
    // std::rotate(frameData[row], frameData[row] + (x % WIDTH),
    //             frameData[row] + WIDTH - 1);
  }

  for (size_t i = 0; i < HEIGHT; i++) {
    memcpy(frameData[(i + y) % HEIGHT], temp[i], WIDTH);
  }
}
