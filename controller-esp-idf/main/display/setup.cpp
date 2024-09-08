#include "setup.hpp"
#include "esp_timer.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <cstring>

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