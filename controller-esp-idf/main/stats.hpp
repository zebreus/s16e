#pragma once

struct Stats {
  unsigned long long pixelsDrawn = 0;
  unsigned long long wagenHaltToggled = 0;
  unsigned long long wagenHaltOnMillis = 0;
  unsigned long long maxConnections = 0;
  unsigned long long currentConnections = 0;
};

extern Stats stats;