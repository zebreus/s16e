#pragma once

#include "../config.hpp"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "srgbFunction.hpp"
#include <cstring>
#include <stdio.h>
#include <string.h>

class Display {
  unsigned char frameData[HEIGHT][WIDTH];

  // Which subframe we are currently on
  int currentSubframe = 0;

  void displaySubframe();

public:
  Display();
  // Render the frame once
  void show();
  void setPixel(unsigned char x, unsigned char y, unsigned char brightness,
                unsigned char transparency);
  unsigned char getPixel(unsigned char x, unsigned char y);
};

extern Display display;

void setupDisplay();
void writeChar(unsigned char data);
void writeChars(const unsigned char *data, int size);