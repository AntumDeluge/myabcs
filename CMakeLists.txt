cmake_minimum_required(VERSION 2.6.2)
project(MyABCs)

if(WIN32)
	set(PLATFORM "win32")
	# FIXME: need to detect arch for systems other than MinGW/MSYS/Cygwin
	# detect MinGW
	if(DEFINED ENV{MSYSTEM})
		if("$ENV{MSYSTEM}" STREQUAL "MINGW32")
			set(ARCH "i686")
		elseif("$ENV{MSYSTEM}" STREQUAL "MINGW64")
			set(ARCH "x86_64")
		else()
			execute_process(COMMAND uname -m OUTPUT_VARIABLE ARCH)
		endif()
	endif()
else()
	execute_process(COMMAND uname -o COMMAND tr A-Z a-z OUTPUT_VARIABLE PLATFORM)
	execute_process(COMMAND uname -m OUTPUT_VARIABLE ARCH)
endif()

string(STRIP "${PLATFORM}" PLATFORM)
string(STRIP "${ARCH}" ARCH)

set(BUILD_STRING "${PLATFORM}-${ARCH}")
set(BUILTIN_LIBPREFIX "${CMAKE_SOURCE_DIR}/libraries/libprefix-${BUILD_STRING}" CACHE STRING "Prefix to built-in libraries")

set(USE_BUILTIN OFF CACHE BOOL "Prioritize built-in libs over system ones")

# FIXME: should use PROJECT_SOURCE_DIR over CMAKE_SOURCE_DIR???
include_directories("${CMAKE_SOURCE_DIR}/include")
if(USE_BUILTIN)
	include_directories("${BUILTIN_LIBPREFIX}/include")
	link_directories("${BUILTIN_LIBPREFIX}/lib")
	set(ENV{PATH} "${BUILTIN_LIBPREFIX}/bin;$ENV{PATH}")
	set(ENV{PKG_CONFIG_PATH} "${BUILTIN_LIBPREFIX}/lib/pkgconfig;${BUILTIN_LIBPREFIX}/share/pkgconfig;$ENV{PKG_CONFIG_PATH}")
	set(CMAKE_PREFIX_PATH "${BUILTIN_LIBPREFIX}")
endif(USE_BUILTIN)

# FIXME: exe in BUILTIN_LIBPREFIX/bin should be priority
include(FindPkgConfig)

# wxWidgets & wxSVG
find_package(wxWidgets COMPONENTS core base REQUIRED)
include("${wxWidgets_USE_FILE}")
pkg_search_module(WXSVG REQUIRED libwxsvg)

# SDL2 & SDL2_mixer
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})
