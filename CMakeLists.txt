cmake_minimum_required(VERSION 2.6.2)
project(MyABCs)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

INCLUDE(FindPkgConfig)

# wxWidgets
find_package(wxWidgets COMPONENTS core base REQUIRED)
include("${wxWidgets_USE_FILE}")

# SDL2 & SDL2_mixer
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)

include_directories(${PROJECT_SRC_DIR}/include ${SDL2_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})

set(
	SRC_FILES src/main.cpp src/abc.cpp src/category.cpp src/gnrcabt.cpp
)

if(WIN32)
	set(SRCS ${SRCS} MyABCs.rc)
endif(WIN32)

set(EXECUTABLE "myabcs")
add_executable(${EXECUTABLE} ${SRC_FILES})
target_link_libraries(${EXECUTABLE} ${wxWidgets_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
