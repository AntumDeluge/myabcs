
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

# app is portable by default
set(PORTABLE ON CACHE BOOL "App searches for data files in path relative to executable")

set(USE_BUNDLED OFF CACHE BOOL "Prioritize bundled libs over system ones")

set(STATIC OFF CACHE BOOL "Link statically to libgcc & libstdc++")
if(STATIC)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()
