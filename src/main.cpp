#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#define LUA_LOCAL_PATH LUA_LOCAL_ROOT "/?.lua;" LUA_LOCAL_ROOT "/?"
#define LUA_LOCAL_CPATH LUA_LOCAL_ROOT "/?.so;" LUA_LOCAL_ROOT "/?"

int main(int, char *[])
{
  setenv("LUA_PATH", LUA_LOCAL_PATH, 1);
  setenv("LUA_CPATH", LUA_LOCAL_CPATH, 1);

  sol::state lua;
  // open some common libraries
  lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::io);

  lua.script_file(LUA_LOCAL_ROOT "/app/main.lua");

  std::cout << std::endl;

  return 0;
}
