# CppLuaPkg

CMake driven Conan/LuaDist packages with minimal C++/Lua/Moonscript project

* Single CMakeLists.txt with conan and LuaDist integration.
* Install LuaJIT C++ library via conan, as in conanfile.txt.
* Install md5 Lua library via LuaDist, as in luadistfile.txt.
* Transpile a moonscript to print the result of a simple md5 calculation.
* Execute a pure Lua script as the main entry point.
* Use sol2 library for managing embedded Lua (unavailable from package managers, but header-only so just copied).
