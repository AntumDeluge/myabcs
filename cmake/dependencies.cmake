
include(FindPkgConfig)

# SDL2
#find_package(SDL2)
# prefer pkg-config because it sets SDL2_VERSION
pkg_search_module(SDL2 sdl2)
if(NOT SDL2_VERSION)
	if(USE_BUNDLED)
		message("\nBuilding SDL2, this could take a while")
		# FIXME: this will fail if there is whitespace in LIB_BUILD_SCRIPT
		execute_process(COMMAND bash -c "${LIB_BUILD_SCRIPT} ${BUILD_STRING} SDL2")
		pkg_search_module(SDL2 REQUIRED sdl2)
	else()
		message(FATAL_ERROR "Please install SDL2 (https://libsdl.org/) or enable USE_BUNDLED")
	endif()
endif()

# SDL2_mixer
pkg_search_module(SDL2MIXER SDL2_mixer>=2.0.0)
if(NOT SDL2MIXER_VERSION)
	if(USE_BUNDLED)
		message("\nBuilding SDL2_mixer, this could take a while")
		# FIXME: this will fail if there is whitespace in LIB_BUILD_SCRIPT
		execute_process(COMMAND bash -c "${LIB_BUILD_SCRIPT} ${BUILD_STRING} SDL2_mixer")
		pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)
	else()
		message(FATAL_ERROR "Please install SDL2_mixer (https://libsdl.org/projects/SDL_mixer/) or enable USE_BUNDLED")
	endif()
endif()

# follow DEBUG option
set(wxWidgets_USE_DEBUG OFF CACHE BOOL "Overridden by 'CMAKE_BUILD_TYPE'" FORCE)
if(DEBUG)
	set(wxWidgets_USE_DEBUG ON CACHE BOOL "Overridden by 'CMAKE_BUILD_TYPE'" FORCE)
endif()

# follow STATIC option
set(wxWidgets_USE_STATIC OFF CACHE BOOL "Overridden by 'STATIC'" FORCE)
if(STATIC)
	set(wxWidgets_USE_STATIC ON CACHE BOOL "Overridden by 'STATIC'" FORCE)
endif()

mark_as_advanced(FORCE wxWidgets_USE_DEBUG)
mark_as_advanced(FORCE wxWidgets_USE_STATIC)

# wxWidgets
set(WX_MIN_VER 3.1)
find_package(wxWidgets COMPONENTS core base richtext)
if(NOT wxWidgets_VERSION_STRING)
	if(USE_BUNDLED)
		message("\nBuilding wxWidgets, this could take a while")
		# FIXME: this will fail if there is whitespace in LIB_BUILD_SCRIPT
		execute_process(COMMAND bash -c "${LIB_BUILD_SCRIPT} ${BUILD_STRING} wxWidgets")
		find_package(wxWidgets COMPONENTS core base richtext REQUIRED)
	else()
		message(FATAL_ERROR "Please install wxWidgets (https://www.wxwidgets.org/) version ${WX_MIN_VER} or newer or enable USE_BUNDLED")
	endif()
elseif(wxWidgets_VERSION_STRING LESS WX_MIN_VER)
	message(FATAL_ERROR "Build requires at least wxWidgets version ${WX_MIN_VER}, found ${wxWidgets_VERSION_STRING}\nAlternatively, you can enable USE_BUNDLED")
endif()
include("${wxWidgets_USE_FILE}")
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	# redirect stdout to console
	string(REPLACE "subsystem,windows" "subsystem,console" wxWidgets_LIBRARIES "${wxWidgets_LIBRARIES}")
endif()

# wxSVG
pkg_search_module(WXSVG libwxsvg)
if(NOT WXSVG_VERSION)
	if(USE_BUNDLED)
		message("\nBuilding wxSVG, this could take a while")
		# FIXME: this will fail if there is whitespace in LIB_BUILD_SCRIPT
		execute_process(COMMAND bash -c "${LIB_BUILD_SCRIPT} ${BUILD_STRING} wxSVG")
		pkg_search_module(WXSVG REQUIRED libwxsvg)
	else()
		message(FATAL_ERROR "Please install wxSVG (http://wxsvg.sf.net/) or enable USE_BUNDLED")
	endif()
endif()
add_compile_definitions(WXSVG_VERSION="${WXSVG_VERSION}")

# threading library
set(THREADS_PREFER_PTHREAD_FLAG "ON")
find_package(Threads REQUIRED)
if(NOT CMAKE_USE_PTHREADS_INIT)
	message(FATAL_ERROR "Required Posix threads library not found")
endif()

include_directories("${SDL2_INCLUDE_DIRS}" "${SDL2MIXER_INCLUDE_DIRS}")

# FFmpeg is used for conveting default flac audio files to other formats
find_program(FFMPEG ffmpeg)
