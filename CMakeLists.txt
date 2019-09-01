cmake_minimum_required(VERSION 2.6.2)
project(MyABCs)
set(PROJECT_VERSION 0.4.7)
set(PROJECT_VERSION_DEV 1)
set(PROJECT_HOMEPAGE_URL "https://antumdeluge.github.io/myabcs")

add_compile_definitions(VERSION="${PROJECT_VERSION}")
if(${PROJECT_VERSION_DEV} GREATER 0)
	add_compile_definitions(VER_DEV="${PROJECT_VERSION_DEV}")
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

# FIXME: exe in BUILTIN_LIBPREFIX/bin should be priority
include(FindPkgConfig)

# wxWidgets & wxSVG
find_package(wxWidgets COMPONENTS core base REQUIRED)
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

# TODO: copy license & other data files
# FIXME: target executable is deleted of add_custom_command execution fails

# copy images
add_custom_command(
	TARGET ${EXECUTABLE} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/image" "${CMAKE_BINARY_DIR}/data/image"
	COMMENT "Copying image data"
)

set(VALID_AFMTS flac vorbis oga ogg mp3)
file(GLOB_RECURSE RESOURCES_FLAC "${CMAKE_SOURCE_DIR}/data/audio/*.flac")

# audio output format
find_program(FFMPEG ffmpeg)
if(NOT FFMPEG)
	message("FFmpeg not found")
	add_custom_command(
		TARGET ${EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/audio" "${CMAKE_BINARY_DIR}/data/audio"
		COMMENT "Copying audio data"
	)
else()
	message("Found FFmpeg")
	# default to OGG/Vorbis
	set(AFORMAT "vorbis" CACHE STRING "Audio format")
	if("${AFORMAT}" STREQUAL "flac")
		# same as if ffmpeg not found
		add_custom_command(
			TARGET ${EXECUTABLE} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/audio" "${CMAKE_BINARY_DIR}/data/audio"
			COMMENT "Copying audio data"
		)
	else()
		if("${AFORMAT}" STREQUAL "vorbis" OR "${AFORMAT}" STREQUAL "oga" OR "${AFORMAT}" STREQUAL "ogg")
			set(ACODEC libvorbis)
			set(AEXT oga)
			set(ABITRATE 96k)
		elseif("${AFORMAT}" STREQUAL "mp3")
			set(ACODEC libmp3lame)
			set(AEXT mp3)
			set(ABITRATE 128k)
		else()
			message(FATAL_ERROR "Unrecognized audio format (AFORMAT): ${AFORMAT}\nValid formats: ${VALID_AFMTS}")
		endif()

		# FIXME: automatically detect subdirs to create
		file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/data/audio/alpha")
		file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/data/audio/effect")

		foreach(FLAC ${RESOURCES_FLAC})
			string(REPLACE "${CMAKE_SOURCE_DIR}/" "" FLAC "${FLAC}")
			string(REPLACE ".flac" "" FLAC "${FLAC}")
			add_custom_command(
				TARGET ${EXECUTABLE} POST_BUILD
				COMMAND ${FFMPEG} -y -hide_banner -i "${CMAKE_SOURCE_DIR}/${FLAC}.flac" -c:a ${ACODEC} -b:a ${ABITRATE} -ar 44100 "${CMAKE_BINARY_DIR}/${FLAC}.${AEXT}"
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
endif()

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
