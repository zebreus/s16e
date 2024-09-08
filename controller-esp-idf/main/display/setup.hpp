#pragma once

#include "../config.hpp"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <cstring>
#include <stdio.h>
#include <string.h>

// This gets buggy sometimes
void rotate(unsigned char x, unsigned char y);
void drawColumn(char character, size_t position);
// Write 8 bits
unsigned char drawCharacter(char character, uint8_t position);
void drawString(const char *string, uint8_t position);
// void displayFrame();