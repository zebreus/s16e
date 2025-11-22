#include "State.hpp"
#include "../display/Display.hpp"
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
  
  // Handle scrolling text
  if (scrollingEnabled) {
    scrollingCounter++;
    if (scrollingCounter >= scrollingSpeed) {
      scrollingCounter = 0;
      scrollingOffset = (scrollingOffset + 1) % scrollingTextWidth;
      renderScrollingText();
    }
  }
}

void State::renderScrollingText() {
  // Clear the display
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      display.setPixel(x, y, 0, 255);
    }
  }
  
  // Draw text with scrolling offset
  // For seamless looping, we may need to draw the text up to 2 times
  int baseX = -scrollingOffset;
  
  // Determine how many instances we need to draw
  int instances = (scrollingTextWidth >= WIDTH) ? 1 : 2;
  
  for (int instance = 0; instance < instances; instance++) {
    int virtualX = baseX + (instance * scrollingTextWidth);
    
    for (size_t i = 0; i < scrollingTextLength; i++) {
      // Calculate actual screen position with proper wrapping
      if (virtualX >= 0 && virtualX < WIDTH) {
        auto width = display.drawCharacter(scrollingText[i], (unsigned char)virtualX, 0);
        virtualX += width;
        
        if (virtualX >= 0 && virtualX < WIDTH) {
          display.drawCharacter(' ', (unsigned char)virtualX, 0);
        }
        virtualX += 1;
      } else {
        // Character is off-screen, but still need to advance position
        static OctafontRegular font;
        auto width = font.get_width(scrollingText[i]);
        if (width == 255) {
          width = font.get_width('?');
        }
        virtualX += width + 1;
      }
    }
  }
}