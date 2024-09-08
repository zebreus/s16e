#include "Display.hpp"
#include "../config.hpp"
#include "../stats.hpp"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "fonts/octafont-regular.hpp"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include <array>
#include <cstring>
#include <stdio.h>
#include <string.h>

void enableOutput() { gpio_set_level(OUTPUT_ENABLE, 0); }
void disableOutput() { gpio_set_level(OUTPUT_ENABLE, 1); }
void shiftRowToOutput() { gpio_set_level(REGISTER_CLOCK, 1); }
void resetShiftRowToOutput() { gpio_set_level(REGISTER_CLOCK, 0); }
void enableRow(int row) { gpio_set_level(ROW_PINS[(row - 1) % HEIGHT], 0); }
void disableRow(int row) { gpio_set_level(ROW_PINS[(row - 1) % HEIGHT], 1); }

void writeRow(unsigned char (&rowData)[WIDTH], unsigned int threshold) {
  unsigned char srgbRow[WIDTH];
  for (unsigned char i = 0; i < WIDTH; i++) {
    srgbRow[i] = srgbToLinear(rowData[i]);
  }

  unsigned char bitstream[WIDTH / 8];
  for (unsigned char column = 0; column < WIDTH; column += 8) {
    bitstream[column / 8] =
        (srgbRow[column + 0] >= threshold ? 0b10000000 : 0) |
        (srgbRow[column + 1] >= threshold ? 0b01000000 : 0) |
        (srgbRow[column + 2] >= threshold ? 0b00100000 : 0) |
        (srgbRow[column + 3] >= threshold ? 0b00010000 : 0) |
        (srgbRow[column + 4] >= threshold ? 0b00001000 : 0) |
        (srgbRow[column + 5] >= threshold ? 0b00000100 : 0) |
        (srgbRow[column + 6] >= threshold ? 0b00000010 : 0) |
        (srgbRow[column + 7] >= threshold ? 0b00000001 : 0);
  }
  writeChars(rowData, (WIDTH / 8));
}

void Display::show() {
  //   serialStep();
  // networkStep();

  vTaskSuspendAll();
  for (size_t row = 0; row < HEIGHT; row++) {
    // Switch the previous row on
    //    taskENTER_CRITICAL(&myMutex);

    // setTurnOffInterrupt((row - 1) % HEIGHT);
    for (auto i = 0; i < SUBFRAMES_PER_FRAME; i++) {
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

      currentSubframe = (currentSubframe + 1) % SUBFRAMES;
      unsigned char threshold =
          (int)(255.0 * (currentSubframe + 0.5) / SUBFRAMES);

      writeRow(frameData[row], threshold);
    }

    // Push data
    disableRow(row);
  }
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
