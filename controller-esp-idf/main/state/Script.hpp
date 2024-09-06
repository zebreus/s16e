#include "../config.hpp"
#include "esp_log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>

extern "C" void luaHook(lua_State *L, lua_Debug *ar);

class Script {
  lua_State *stack;

  void initStack();

public:
  Script();
  ~Script();
  Script(const Script &other) = delete;
  Script &operator=(Script &other) = delete;
  Script(Script &&other);
  Script &operator=(Script &&other);

  void loadScript(const char *program);
  void executeScript();
};