#pragma once

#include "State.hpp"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <cstring>

extern bool wagenHaltOn;
extern unsigned long long wagenHaltTurnedOnAt;

void printHelp(State &state);
void printColorAt(State &state, unsigned char x, unsigned char y);
void stepState(State &state, unsigned char c);
bool isStateSleeping(State &state);