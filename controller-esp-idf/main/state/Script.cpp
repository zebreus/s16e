#include "Script.hpp"
#include "../display/Display.hpp"
#include "../display/setup.hpp"
#include "../stats.hpp"
#include "lauxlib.h"
#include "lua.h"
#include "setup.hpp"

extern "C" void luaHook(lua_State *L, lua_Debug *ar) {
  ESP_LOGE(LUA_SCRIPTING_LOG_TAG, "lua hook triggered");
  if (ar->event != LUA_HOOKCOUNT) {
    luaL_error(L, "You can only run %i lua instructions per frame.",
               LUA_MAX_INSTRUCTIONS_PER_FRAME);
    // lua_yieldk(L, 0, 0, 0);
  }
}

// setPixel(x, y, color, transparency)
extern "C" int setPixel(lua_State *L) {
  int x = luaL_checkinteger(L, 1);
  luaL_argcheck(L, x >= 0 && x < 120, 1, "Out of range 0..120");
  int y = luaL_checkinteger(L, 2);
  luaL_argcheck(L, y >= 0 && y < 8, 2, "Out of range 0..8");
  int brightness = luaL_checkinteger(L, 3);
  luaL_argcheck(L, brightness >= 0 && brightness < 256, 3,
                "Out of range 0..256");
  int transparency = luaL_checkinteger(L, 4);
  luaL_argcheck(L, transparency >= 0 && transparency < 256, 4,
                "Out of range 0..256");

  display.setPixel(x, y, brightness, transparency);

  return 0;
}

// setPixel(x, y, color, transparency)
extern "C" int luaSetWagenHalt(lua_State *L) {
  bool newWagenHalt = lua_toboolean(L, 1);
  setWagenHalt(newWagenHalt);

  return 0;
}

// setPixel(x, y, color, transparency)
extern "C" int luaGetWagenHalt(lua_State *L) {
  lua_pushboolean(L, wagenHaltOn);

  return 1;
}

// setPixel(x, y, color, transparency)
extern "C" int getPixel(lua_State *L) {
  int x = luaL_checkinteger(L, 0);
  luaL_argcheck(L, x >= 0 && x < 120, 1, "Out of range 0..120");
  int y = luaL_checkinteger(L, 1);
  luaL_argcheck(L, y >= 0 && y < 8, 2, "Out of range 0..8");

  unsigned char value = display.getPixel(x, y);

  lua_pushinteger(L, value);
  return 1;
}

void Script::initStack() {
  if (stack == nullptr) {
    stack = luaL_newstate();
    luaL_openlibs(stack);
    lua_pushcfunction(stack, setPixel);
    lua_setglobal(stack, "setPixel");
    lua_pushcfunction(stack, getPixel);
    lua_setglobal(stack, "getPixel");
    lua_pushcfunction(stack, luaGetWagenHalt);
    lua_setglobal(stack, "getWagenHalt");
    lua_pushcfunction(stack, luaSetWagenHalt);
    lua_setglobal(stack, "setWagenHalt");
  }
}

Script::Script() : stack(nullptr) { initStack(); }
Script::~Script() {
  if (stack != nullptr) {
    lua_close(stack);
  }
}
Script::Script(Script &&other) {
  this->stack = other.stack;
  // TODO: Support setting state to a nullptr to avoid initializing and
  // instantly deleting the stack
  other.stack = nullptr;
};
Script &Script::operator=(Script &&other) {
  this->stack = other.stack;
  other.stack = nullptr;
  return *this;
};

void Script::loadScript(const char *program) {
  if (stack == nullptr) {
    ESP_LOGE(LUA_SCRIPTING_LOG_TAG,
             "Tried to load script in moved from Script object.");
    return;
  }
  int status = luaL_loadstring(stack, program);
  if (status != LUA_OK) {
    const char *msg = lua_tostring(stack, -1);
    ESP_LOGI(LUA_SCRIPTING_LOG_TAG, "Failed to load lua program; %s\n", msg);
    lua_pop(stack, 1); /* remove message */
    return;
  }
  lua_setglobal(stack, "__lua_entrypoint");
}

void Script::executeScript() {
  if (stack == nullptr) {
    ESP_LOGE(LUA_SCRIPTING_LOG_TAG,
             "Tried to execute moved from Script object.");
    return;
  }
  lua_sethook(stack, luaHook, LUA_MASKCOUNT, LUA_MAX_INSTRUCTIONS_PER_FRAME);
  lua_getglobal(stack, "__lua_entrypoint");
  auto status = lua_pcall(stack, 0, 0, 0);
  if (status != LUA_OK) {
    const char *msg = lua_tostring(stack, -1);
    ESP_LOGI(LUA_SCRIPTING_LOG_TAG, "Lua execution failed: %s", msg);
    // lua_writestring(msg, strlen(msg));
    // lua_writeline();
    lua_pop(stack, 1); /* remove message */
  }
}

void Script::setOutput(State &outputState) {
  // TODO: Figure out how to do this.
}