cmake_minimum_required(VERSION 3.12.4)
project(MyABCs VERSION 0.4.7 HOMEPAGE_URL "https://antumdeluge.github.io/myabcs")
set(PROJECT_VERSION_DEV 1)

add_compile_definitions(VERSION="${PROJECT_VERSION}")
if(${PROJECT_VERSION_DEV} GREATER 0)
	add_compile_definitions(VER_DEV="${PROJECT_VERSION_DEV}")
endif()

if(NOT CMAKE_BUILD_TYPE)
	# default build is Release
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Release Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("Debugging enabled")
	add_compile_definitions(DEBUG=1)
endif()

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

include(FindPkgConfig)

# wxWidgets & wxSVG
set(WX_REQUIRED_VER 3.1)
find_package(wxWidgets COMPONENTS core base REQUIRED)
if(wxWidgets_VERSION_STRING LESS WX_REQUIRED_VER)
	message(FATAL_ERROR "Build requires at least wxWidgets version ${WX_REQUIRED_VER}, found ${wxWidgets_VERSION_STRING}")
endif()
include("${wxWidgets_USE_FILE}")
pkg_search_module(WXSVG REQUIRED libwxsvg)

# SDL2 & SDL2_mixer
find_package(SDL2 REQUIRED)
# TODO: use pkg-config as failsafe
#pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)

include_directories("${SDL2_INCLUDE_DIRS}" "${SDL2MIXER_INCLUDE_DIRS}")

# retrieve source files
file(GLOB FILES_C "${CMAKE_SOURCE_DIR}/src/*.cpp")
if(WIN32)
	list(APPEND FILES_C "${CMAKE_SOURCE_DIR}/win_resources.rc")
endif()

set(EXECUTABLE "myabcs")
add_executable(${EXECUTABLE} ${FILES_C})
# FIXME: way to use/copy without explicit declaration of suffix???
if(WIN32)
	set(EXE_SUFFIX ".exe")
endif()

if(USE_BUILTIN)
	target_link_libraries(${EXECUTABLE} ${wxWidgets_LIBRARIES} ${WXSVG_STATIC_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES})
else()
	target_link_libraries(${EXECUTABLE} ${wxWidgets_LIBRARIES} ${WXSVG_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES})
endif()

# FIXME: target executable is deleted of add_custom_command execution fails

# clean data directory before build
add_custom_command(
	TARGET ${EXECUTABLE} PRE_BUILD
	# FIXME: need platform-independent method
	COMMAND rm -rf "${CMAKE_BINARY_DIR}/data"
	# FIXME: automatically detect subdirs to create
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/data/audio/alpha"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/data/audio/effect"
	COMMENT "Preparing data directory"
)

# copy images
add_custom_command(
	TARGET ${EXECUTABLE} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/image" "${CMAKE_BINARY_DIR}/data/image"
	COMMENT "Copying image data"
)

set(VALID_AFMTS copy flac vorbis oga ogg mp3 pcm wav)
set(AFMT_LIST copy vorbis mp3 pcm) # used for drop-down list in CMake GUI
file(GLOB_RECURSE RESOURCES_FLAC "${CMAKE_SOURCE_DIR}/data/audio/*.flac")

# audio output format
find_program(FFMPEG ffmpeg)
if(NOT FFMPEG)
	message(WARNING "FFmpeg not found: converting audio not supported")
	add_custom_command(
		TARGET ${EXECUTABLE} POST_BUILD
		# FIXME: should only copy .flac files
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/audio" "${CMAKE_BINARY_DIR}/data/audio"
		COMMENT "Copying audio data"
	)
else()
	message("Found FFmpeg")
	# default to OGG/Vorbis
	set(AUDIO_FORMAT "vorbis" CACHE STRING "Audio format (set to 'copy' to keep uncompressed FLAC audio files)")
	set_property(CACHE AUDIO_FORMAT PROPERTY STRINGS ${AFMT_LIST})
	if(AUDIO_FORMAT STREQUAL "copy" OR AUDIO_FORMAT STREQUAL "flac")
		# same as if ffmpeg not found
		add_custom_command(
			TARGET ${EXECUTABLE} POST_BUILD
			# FIXME: should only copy .flac files
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/audio" "${CMAKE_BINARY_DIR}/data/audio"
			COMMENT "Copying audio data"
		)
	else()
		if(AUDIO_FORMAT STREQUAL "vorbis" OR AUDIO_FORMAT STREQUAL "oga" OR AUDIO_FORMAT STREQUAL "ogg")
			set(ACODEC libvorbis)
			set(AEXT oga)
			set(AB -b:a)
			set(ABITRATE 96k)
		elseif(AUDIO_FORMAT STREQUAL "mp3")
			set(ACODEC libmp3lame)
			set(AEXT mp3)
			set(AB -b:a)
			set(ABITRATE 128k)
		elseif(AUDIO_FORMAT STREQUAL "pcm" OR AUDIO_FORMAT STREQUAL "wav")
			set(ACODEC pcm_s16le)
			set(AEXT wav)
		else()
			message(FATAL_ERROR "Unrecognized audio format: ${AUDIO_FORMAT}\nValid formats: ${VALID_AFMTS}")
		endif()

		foreach(FLAC ${RESOURCES_FLAC})
			string(REPLACE "${CMAKE_SOURCE_DIR}/" "" FLAC "${FLAC}")
			string(REPLACE ".flac" "" FLAC "${FLAC}")
			add_custom_command(
				TARGET ${EXECUTABLE} POST_BUILD
				COMMAND ${FFMPEG} -y -hide_banner -loglevel warning -i "${CMAKE_SOURCE_DIR}/${FLAC}.flac" -c:a ${ACODEC} ${AB} ${ABITRATE} -ar 44100 "${CMAKE_BINARY_DIR}/${FLAC}.${AEXT}"
				COMMENT "Converting audio: ${FLAC}"
			)
		endforeach()
	endif()
endif()

if(WIN32 AND USE_BUILTIN)
	file(GLOB RESOURCES_DLL "${BUILTIN_LIBPREFIX}/bin/*.dll")
	add_custom_command(
		TARGET ${EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCES_DLL} "${CMAKE_BINARY_DIR}"
		COMMENT "Copying DLLs"
	)
	# unused DLLs
	# FIXME: doesn't work (list needs to be generated during TARGET build)
	# FIXME: use case-insensitive method
	file(GLOB UNUSED_DLL "wx*_gl_*.dll") # "libasprintf*.dll" "libgettext*.dll")
	# DEBUG:
	message("Unused DLLs: ${UNUSED_DLL}")
	add_custom_command(
		TARGET ${EXECUTABLE} POST_BUILD
		COMMAND rm -vf ${UNUSED_DLL}
		COMMENT "Deleting unused DLLs"
	)
endif()

# platform-dependent files preparation

# X11
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/x11")
set(SH_CONTENTS "#!/bin/sh\n\n\"${CMAKE_INSTALL_PREFIX}/share/myabcs/myabcs\"\n")
file(WRITE "${CMAKE_BINARY_DIR}/x11/myabcs" "${SH_CONTENTS}")
# use Unix LF line endings in shell scripts
configure_file(
	"${CMAKE_BINARY_DIR}/x11/myabcs" "${CMAKE_BINARY_DIR}/x11/myabcs"
	NEWLINE_STYLE LF
)
# set executable bit
file(
	COPY "${CMAKE_BINARY_DIR}/x11/myabcs"
	DESTINATION "${CMAKE_BINARY_DIR}/x11"
	# XXX: what is the difference between FILE_PERMISSIONS & PERMISSIONS
	FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
file(
	COPY "${CMAKE_SOURCE_DIR}/data/resource/logo/myabcs_single_block.svg"
	DESTINATION "${CMAKE_BINARY_DIR}/x11"
)
file(RENAME "${CMAKE_BINARY_DIR}/x11/myabcs_single_block.svg" "${CMAKE_BINARY_DIR}/x11/myabcs.svg")
file(
	COPY "${CMAKE_SOURCE_DIR}/data/resource/x11/myabcs.desktop"
	DESTINATION "${CMAKE_BINARY_DIR}/x11"
)

add_custom_target(clean-stage
	# FIXME: need platform independent method to delete directory
	COMMAND rm -rf "${CMAKE_BINARY_DIR}/stage"
	COMMENT "Cleaning stage directory"
)

add_custom_target(stage
	DEPENDS clean-stage
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/stage/myabcs"
	COMMAND ${CMAKE_COMMAND} -E copy "${EXECUTABLE}${EXE_SUFFIX}" "${CMAKE_BINARY_DIR}/stage/myabcs"
	# FIXME: need to copy files with .DLL suffix as well
	COMMAND ${CMAKE_COMMAND} -E copy "*.dll" "${CMAKE_BINARY_DIR}/stage/myabcs"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "data" "${CMAKE_BINARY_DIR}/stage/myabcs/data"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/doc" "${CMAKE_BINARY_DIR}/stage/myabcs/doc"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/CHANGES.txt" "${CMAKE_BINARY_DIR}/stage/myabcs"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE.txt" "${CMAKE_BINARY_DIR}/stage/myabcs"
	COMMENT "Staging files"
)

if(WIN32 AND "${ARCH}" STREQUAL "i686")
	set(REL_SUFFIX "win32")
elseif(WIN32 AND "${ARCH}" STREQUAL "x86_64")
	set(REL_SUFFIX "win64")
else()
	set(REL_SUFFIX "${PLATFORM}-${ARCH}")
endif()

add_custom_target(package
	${CMAKE_COMMAND} -E tar "cfv" "../${PROJECT_NAME}-${PROJECT_VERSION}-${REL_SUFFIX}.zip" --format=zip "${CMAKE_BINARY_DIR}/stage/myabcs"
	DEPENDS stage
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/stage"
	COMMENT "Creating release package: ${PROJECT_NAME} ${PROJECT_VERSION}"
)

# installation
set(DIR_BIN "${CMAKE_INSTALL_PREFIX}/bin")
set(DIR_DATA "${CMAKE_INSTALL_PREFIX}/share/myabcs")

install(
	TARGETS ${EXECUTABLE}
	RUNTIME DESTINATION "${DIR_DATA}"
)
install(
	DIRECTORY "${CMAKE_BINARY_DIR}/data" "${CMAKE_SOURCE_DIR}/doc"
	DESTINATION "${DIR_DATA}"
)
install(
	FILES "${CMAKE_SOURCE_DIR}/CHANGES.txt" "${CMAKE_SOURCE_DIR}/LICENSE.txt"
	DESTINATION "${DIR_DATA}"
)

# platform-dependent files installation

if(WIN32)
	# Windows DLLs
	# FIXME: use case-insensitive method
	file(GLOB DLLS "${CMAKE_BINARY_DIR}/*.dll")
	install(
		FILES ${DLLS}
		DESTINATION "${DIR_DATA}"
	)
endif()

# X11 files
install(
	FILES "${CMAKE_BINARY_DIR}/x11/myabcs"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
install(
	FILES "${CMAKE_BINARY_DIR}/x11/myabcs.svg"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps"
)
install(
	FILES "${CMAKE_BINARY_DIR}/x11/myabcs.desktop"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications"
)

add_custom_target(uninstall
	# FIXME: need platform-independent method for removing files & directories
	COMMAND rm -vf
		"${CMAKE_INSTALL_PREFIX}/share/applications/myabcs.desktop"
		"${CMAKE_INSTALL_PREFIX}/share/pixmaps/myabcs.svg"
		"${CMAKE_INSTALL_PREFIX}/bin/myabcs"
	COMMAND rm -vrf "${CMAKE_INSTALL_PREFIX}/share/myabcs"
)
