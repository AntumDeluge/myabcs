
# app is portable by default
set(PORTABLE ON CACHE BOOL "App searches for data files in path relative to executable")

set(USE_BUNDLED OFF CACHE BOOL "Prioritize bundled libs over system ones")

set(STATIC OFF CACHE BOOL "Link statically to libgcc & libstdc++")
if(STATIC)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()
