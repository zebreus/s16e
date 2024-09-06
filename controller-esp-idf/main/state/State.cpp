#include "State.hpp"
#include <cstdio>
#include <string.h>

int State::print(const std::span<const char> &text) {
  return this->sendingBuffer.write(text);
}

int State::printf(size_t maxLength, const char *format, ...) {
  static char staticBuffer[SEND_QUEUE_SIZE];

  va_list argptr;
  va_start(argptr, format);

  auto length = std::vsnprintf(staticBuffer, maxLength, format, argptr);
  va_end(argptr);
  if (length < 0) {
    return length;
  }

  return print({staticBuffer, (size_t)length});
}

void State::processScripts() {
  if (luaScript.has_value()) {
    luaScript->executeScript();
  }
}