#include <iostream>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <Eigen/Dense>

#define LUA_LOCAL_PATH LUA_LOCAL_ROOT "/?.lua;" LUA_LOCAL_ROOT "/?"
#define LUA_LOCAL_CPATH LUA_LOCAL_ROOT "/?.so;" LUA_LOCAL_ROOT "/?"

int main(int, char *[])
{
  setenv("LUA_PATH", LUA_LOCAL_PATH, 1);
  setenv("LUA_CPATH", LUA_LOCAL_CPATH, 1);

  sol::state lua;
  // open some common libraries
  lua.open_libraries(
      sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::io, sol::lib::os,
      sol::lib::math, sol::lib::coroutine, sol::lib::table, sol::lib::debug);

  lua.safe_script_file(LUA_LOCAL_ROOT "/app/main.lua");

  std::cout << Eigen::Vector3f{1, 2, 3} << std::endl;

  return 0;
}
