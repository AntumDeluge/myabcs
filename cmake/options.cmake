
set(DEBUG ON) # for use internally instead of typing "CMAKE_BUILD_TYPE STREQUAL"
if(NOT CMAKE_BUILD_TYPE STREQUAL Release)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
	add_compile_definitions(DEBUG=1)
else()
	set(DEBUG OFF)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug)

# strip binary for release builds
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ standard")

# app is portable by default
set(PORTABLE ON CACHE BOOL "App searches for data files in path relative to executable")

set(USE_BUNDLED OFF CACHE BOOL "Prioritize bundled libs over system ones")

set(STATIC OFF CACHE BOOL "Link statically to library dependencies")
if(STATIC)
	# XXX: static linking to many packages is broken on MSYS2/MinGW-w64
	# set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

# supported audio formats
set(VALID_AFMTS copy flac vorbis oga ogg mp3 pcm wav)
set(AFMT_LIST copy vorbis mp3 pcm) # used for drop-down list in CMake GUI
