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
  void drawColumn(unsigned char (&columnData)[HEIGHT], unsigned char column);
  void drawRow(unsigned char (&rowData)[WIDTH], unsigned char row);
  unsigned char drawCharacter(char character, unsigned char x, unsigned char y);
  void drawString(const char *string, unsigned char x, unsigned char y);
  unsigned char getPixel(unsigned char x, unsigned char y);

  // This gets buggy sometimes
  void rotate(unsigned char x, unsigned char y);
};

extern Display display;

void setupDisplay();