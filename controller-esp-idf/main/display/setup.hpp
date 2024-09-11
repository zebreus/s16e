#pragma once

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <cstring>

void IRAM_ATTR delayMicroseconds(uint32_t us);