# CppLuaPkg

[CMake](https://cmake.org/) driven [Conan](https://conan.io/) / [LuaDist](http://luadist.org/) packages with minimal 
C++ / [Lua](https://www.lua.org/) / [Moonscript](http://moonscript.org/) demo project, with Lua state handled by 
[sol2](https://github.com/ThePhD/sol2).

* Single CMakeLists.txt with conan integration via [official plugin](https://github.com/conan-io/cmake-conan) and 
LuaDist integration via their `luadist` binary.
* Assumes conan, LuaDist and Moonscript are already available on the system.
* Transpiles Moonscript as part of the build process.
* Lua/Moonscript directory structure is reflected in runtime output dir, so import paths are predictable.
* Demo installs 
    - [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) math library listed in `conanfile.txt` via conan.
    - LuaJIT library in CMakeLists via LuaDist as a dependency for the main executable and subsequent Lua packages.
    - Several Lua libraries and their dependencies (md5, luasocket, and lsqlite3) listed in `luadistfile.txt` via 
    LuaDist.
* Execution of demo will
    - Load/execute Lua via sol2 (C++).
    - Print an md5 hash (Moonscript).
    - HTTP request www.google.com (Lua).
    - Create an in-memory SQLite database and dump its contents (Lua).
    - Print an Eigen vector (C++).

### Limitations / Future work
* Once installed, LuaDist packages cannot be altered or removed except by manually removing generated `.luadist` 
directory.
