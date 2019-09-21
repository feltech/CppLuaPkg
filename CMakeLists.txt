cmake_minimum_required(VERSION 3.14)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/FetchContent.cmake)

project(CppLua)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

###################################
# Install C++ libraries from conan.

# Fetch conan CMake integration
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
	message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
	file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.14/conan.cmake"
		"${CMAKE_BINARY_DIR}/conan.cmake")
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

# Ensure conan is installed
conan_check(REQUIRED)

conan_cmake_run(
	CONANFILE conanfile.txt  # Install from conanfile
	BASIC_SETUP # Run conan_basic_setup
	BUILD missing
	PROFILE default PROFILE_AUTO build_type # Use default profile override with Debug/Release
)
include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)

###################################
# Install Lua libararies from LuaDist.
message(STATUS "Configuring LuaDist packages")
# Find luadist binary.
find_package(LuaDist REQUIRED)
# Specify libraries to install via a "luadistfile.txt" (to mirror "conanfile.txt").
set(LUADIST_FILE "luadistfile.txt" CACHE PATH "File containing newline-separated list of LuaDist packages to install")
# Ensure CMake reconfigures when luadistfile.txt is altered.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${LUADIST_FILE})
# Read the file and clean it up.
file(READ ${LUADIST_FILE} _libs)
string(STRIP ${_libs} _libs)
# Transform newline delimited to semicolon delimited, i.e. a CMake list.
string(REGEX REPLACE "\n" ";" _libs "${_libs}")
# Set path where to put LuaDist cache (includes src files for debug builds)
set(_luadist_cache_root ${CMAKE_CURRENT_SOURCE_DIR}/.luadist/${CMAKE_BUILD_TYPE})
# Set path for C++ libaries
set(RUNTIME_LIBRARY_DIR "lib" CACHE PATH "Location to install libraries relative to the build tree")
# Set path for Lua libaries (lua and shared libs).
set(LUA_LOCAL_ROOT "${RUNTIME_LIBRARY_DIR}/lua" CACHE PATH
	"Location to install Lua libraries relative to the build tree")
set(_lua_local_root_abs "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LUA_LOCAL_ROOT}")
message(STATUS "LuaDist installing ${_libs}")
# Do the install.
execute_process(
	COMMAND ${LUADIST_BIN} ${_luadist_cache_root} install luajit ${_libs}
	-verbose -binary=false -source=true
	-debug  # Keep src when done - useful for debug builds, so we can breakpoint in the library source.
	-DINSTALL_LMOD=${_lua_local_root_abs}
	-DINSTALL_CMOD=${_lua_local_root_abs}
	-DINSTALL_LIB=${RUNTIME_LIBRARY_DIR} #${_lua_local_root_abs}
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_PREFIX_PATH=${_lua_local_root_abs}
)

###################################
# Create main C++ application target.
add_executable(CppLua src/main.cpp)
find_library(LIBLUA lua PATHS ${RUNTIME_LIBRARY_DIR})
# Add ad-hoc 3rd party C++ dependencies.
include_directories(vendor _luadist/include ${_luadist_cache_root}/include)
# Link installed libraries to main target.
target_link_libraries(CppLua ${CONAN_LIBS} ${LIBLUA})
# Add a compile definition to the location of lua libraries in the binary directory.
add_compile_definitions(LUA_LOCAL_ROOT=\"${LUA_LOCAL_ROOT}\")

###################################
# Check for moonscript files.
set(SCRIPTS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE PATH
	"Root directory to search for Lua/Moonscript files")
file(
	GLOB_RECURSE _scripts LIST_DIRECTORIES false RELATIVE ${SCRIPTS_ROOT}
	CONFIGURE_DEPENDS ${SCRIPTS_ROOT}/*.moon
)
# Ensure we can transpile the moonscript.
if (_scripts)
	message(STATUS "Configuring Moonscript transpilation")
	find_program(MOON moonc REQUIRED)
	if ("${MOON}" STREQUAL "MOON-NOTFOUND")
		message(FATAL "moonscript binary (moonc) not found and is required to transpile")
	endif()
	# Add transpilation step to moonscripts.
	foreach (_script ${_scripts})
		get_filename_component(_dir ${_script} DIRECTORY)
		get_filename_component(_basename ${_script} NAME_WE)
		set(_script_path "${SCRIPTS_ROOT}/${_script}")
		# Mirror the source directory structure in the transpiled destination.
		set(_transpiled_path "${_lua_local_root_abs}/${_dir}/${_basename}.lua")
		file(MAKE_DIRECTORY ${_lua_local_root_abs}/${_dir})
		list(APPEND _transpiled_paths ${_transpiled_path})
		add_custom_command(
			OUTPUT ${_transpiled_path} DEPENDS ${_script_path}
			COMMAND ${MOON} -o ${_transpiled_path} ${_script_path}
			COMMENT "Transpiling ${_script} to ${_transpiled_path}"
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	endforeach ()
	# Gather individual moonscript transpilation commands under a target.
	add_custom_target(moonscripts DEPENDS ${_transpiled_paths})
	# Ensure moonscripts are compiled, if required, before building/running the application.
	add_dependencies(CppLua moonscripts)
endif()

###################################
# Handle normal Lua scripts
file(
	GLOB_RECURSE _scripts LIST_DIRECTORIES false RELATIVE ${SCRIPTS_ROOT}
	CONFIGURE_DEPENDS ${SCRIPTS_ROOT}/*.lua
)
#message(STATUS "Found Lua scripts: ${_scripts}")
foreach (_script ${_scripts})
	configure_file(${SCRIPTS_ROOT}/${_script} ${_lua_local_root_abs}/${_script} COPYONLY)
endforeach()

