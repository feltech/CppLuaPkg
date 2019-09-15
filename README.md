# CppLuaPkg

[CMake](https://cmake.org/) driven [Conan](https://conan.io/)/[LuaDist](http://luadist.org/) packages with minimal C++/[Lua](https://www.lua.org/)/[Moonscript](http://moonscript.org/) project

* Single CMakeLists.txt with conan integration via [official plugin](https://github.com/conan-io/cmake-conan) and LuaDist integration via their [CMake driven repository](https://github.com/LuaDist) and CMake's [ExternalProject plugin](https://cmake.org/cmake/help/latest/module/ExternalProject.html).
* Install [LuaJIT C++ library via conan](https://bintray.com/int010h/recipes/luajit:int010h), as in conanfile.txt.
* Install [md5 Lua library via LuaDist](https://github.com/LuaDist/md5), as in luadistfile.txt.
* Transpile a moonscript to print the result of a simple md5 calculation.
* Execute a pure Lua script as the main entry point.
* Use [sol2](https://github.com/ThePhD/sol2) library for managing embedded Lua.
