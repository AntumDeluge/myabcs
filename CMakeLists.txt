cmake_minimum_required(VERSION 3.12.4)
project(MyABCs VERSION 0.4.7 HOMEPAGE_URL "https://antumdeluge.github.io/myabcs")
set(PROJECT_VERSION_DEV 1)

message("")
message(STATUS "Configuring ${PROJECT_NAME} ${PROJECT_VERSION}")

add_compile_definitions(VERSION="${PROJECT_VERSION}")
if(${PROJECT_VERSION_DEV} GREATER 0)
	add_compile_definitions(VER_DEV="${PROJECT_VERSION_DEV}")
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/options.cmake")

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
set(BUNDLED_LIBPREFIX "${CMAKE_SOURCE_DIR}/libraries/libprefix-${BUILD_STRING}" CACHE STRING "Prefix to bundled libraries")

# FIXME: should use PROJECT_SOURCE_DIR over CMAKE_SOURCE_DIR???
include_directories("${CMAKE_SOURCE_DIR}/include")
if(USE_BUNDLED)
	include_directories("${BUNDLED_LIBPREFIX}/include")
	link_directories("${BUNDLED_LIBPREFIX}/lib")
	set(ENV{PATH} "${BUNDLED_LIBPREFIX}/bin;$ENV{PATH}")
	set(ENV{PKG_CONFIG_PATH} "${BUNDLED_LIBPREFIX}/lib/pkgconfig;${BUNDLED_LIBPREFIX}/share/pkgconfig;$ENV{PKG_CONFIG_PATH}")
	set(CMAKE_PREFIX_PATH "${BUNDLED_LIBPREFIX}")
endif(USE_BUNDLED)

set(LIB_BUILD_SCRIPT "${CMAKE_SOURCE_DIR}/libraries/build.sh")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/targets.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/print_config.cmake")
