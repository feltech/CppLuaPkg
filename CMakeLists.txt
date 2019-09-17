cmake_minimum_required(VERSION 3.14)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${CMAKE_ROOT}/Modules/FetchContent.cmake)

project(CppLua)
set(CMAKE_CXX_STANDARD 17)

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


FetchContent_Declare(luadist GIT_REPOSITORY https://github.com/LuaDist/bootstrap)
FetchContent_GetProperties(luadist)
if(NOT luadist_POPULATED)
	FetchContent_Populate(luadist)
endif()
set(LUADIST "${luadist_SOURCE_DIR}/_install/bin/luadist")
set(LUADIST_CACHE ${CMAKE_CURRENT_BINARY_DIR}/_luadist)
set(_dummyluadist ${LUADIST_CACHE}/share/luadist-git/dists/lua-dummy/dist.info)
if(NOT EXISTS ${LUADIST})
	message(STATUS "Configuring LuaDist local repo")
	execute_process(
		COMMAND ${luadist_SOURCE_DIR}/bootstrap
		WORKING_DIRECTORY ${luadist_SOURCE_DIR}
	)
endif()
file(WRITE ${_dummyluadist} "name = \"lua\"\nversion = \"5.1\"\ndepends = {}")
# Install Lua package from LuaDist's repos into build dir.
function(luadist_install _libs)
	foreach(_lib ${_libs})
		if (NOT "${_lib}" STREQUAL "")
			message(STATUS "Installing LuaDist library ${_lib}")
			execute_process(
				COMMAND ${LUADIST} _luadist install ${_lib} -binary=false -source=true -DINSTALL_LMOD=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib/lua -DINSTALL_CMOD=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib/lua -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} "-DCMAKE_PREFIX_PATH=${CONAN_LUAJIT_ROOT};${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" -DINSTALL_LIB=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} -verbose
			)
		endif()
	endforeach()
endfunction()
# Install the libraries.
file(STRINGS luadistfile.txt _libs)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS luadistfile.txt)
luadist_install("${_libs}")
# Path to LuaDist install root.
set(LUA_LOCAL_ROOT lib/lua)
set(LUA_LOCAL_ROOT_ABS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LUA_LOCAL_ROOT}")

###################################
# Create main C++ application target.
add_executable(CppLua src/main.cpp)
# Add ad-hoc 3rd party C++ dependencies (i.e. sol2).
include_directories(vendor)
# Link conan-installed libraries to main target.
target_link_libraries(CppLua ${CONAN_LIBS})
# Add a compile definition to the location of lua libraries in the binary directory.
add_compile_definitions(LUA_LOCAL_ROOT=\"lib/lua\")

###################################
# Check for moonscript files.
set(SCRIPTS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(
	GLOB_RECURSE _scripts LIST_DIRECTORIES false RELATIVE ${SCRIPTS_ROOT}
	CONFIGURE_DEPENDS ${SCRIPTS_ROOT}/*.moon
)
# Ensure we can transpile the moonscript.
if (_scripts)
	message(STATUS "Configuring Moonscript transpilation")
	find_program(MOON moonc REQUIRED)
	if ("${MOON}" STREQUAL "MOON-NOTFOUND")
		message(FATAL "moonscript not found and is required to transpile")
	endif()
	# Add transpilation step to moonscripts.
	foreach (_script ${_scripts})
		get_filename_component(_dir ${_script} DIRECTORY)
		get_filename_component(_basename ${_script} NAME_WE)
		set(_script_path "${SCRIPTS_ROOT}/${_script}")
		# Mirror the source directory structure in the transpiled destination.
		set(_transpiled_path "${LUA_LOCAL_ROOT_ABS}/${_dir}/${_basename}.lua")
		file(MAKE_DIRECTORY ${LUA_LOCAL_ROOT_ABS}/${_dir})
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
	configure_file(${SCRIPTS_ROOT}/${_script} ${LUA_LOCAL_ROOT_ABS}/${_script} COPYONLY)
endforeach()

