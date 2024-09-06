#include "Script.hpp"

extern "C" void luaHook(lua_State *L, lua_Debug *ar) {
  ESP_LOGE(LUA_SCRIPTING_LOG_TAG, "lua hook triggered");
  if (ar->event != LUA_HOOKCOUNT) {
    luaL_error(L, "You can only run %i lua instructions per frame.",
               LUA_MAX_INSTRUCTIONS_PER_FRAME);
    // lua_yieldk(L, 0, 0, 0);
  }
}

void Script::initStack() {
  if (stack == nullptr) {
    stack = luaL_newstate();
    luaL_openlibs(stack);
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
  ESP_LOGI(LUA_SCRIPTING_LOG_TAG, "Lua pcall returned %i", status);
}

void Script::setOutput(State &outputState) {
  // TODO: Figure out how to do this.
}