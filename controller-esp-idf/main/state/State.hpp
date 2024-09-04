#pragma once

#include "../RingBuffer.hpp"
#include "../config.hpp"
#include <cstdarg>
#include <cstddef>
#include <cstdio>

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

  // Buffer for outgoing data
  RingBuffer<SEND_QUEUE_SIZE> sendingBuffer;

  // Sleep until this time when sleep is set
  unsigned int sleepUntil = 0;
  bool sleep = false;

  int print(const std::span<const char> &text);
  int printf(size_t maxLength, const char *format, ...);
};