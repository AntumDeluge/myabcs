
# retrieve source files
file(GLOB FILES_C "${CMAKE_SOURCE_DIR}/src/*.cpp")
if(WIN32)
	list(APPEND FILES_C "${CMAKE_SOURCE_DIR}/win_resources.rc")
endif()

set(EXECUTABLE "myabcs")
add_executable(${EXECUTABLE} ${FILES_C})
if(WIN32)
	set(EXE_SUFFIX ".exe")
endif()

if(PORTABLE)
	# ensure data root not set
	unset(DATAROOT CACHE)
	remove_definitions(-DDATAROOT)
else()
	# root directory where app will search for data files
	set(DATAROOT "${CMAKE_INSTALL_PREFIX}/share/myabcs" CACHE PATH "Root directory where app will search for data files")
	add_compile_definitions(DATAROOT="${DATAROOT}")
endif()

target_link_libraries(${EXECUTABLE} ${LIBS} Threads::Threads)

# FIXME: target executable is deleted if add_custom_command execution fails

# clean data directory before build
add_custom_command(
	TARGET ${EXECUTABLE} PRE_BUILD
	# FIXME: need platform-independent method
	COMMAND rm -rf "${CMAKE_BINARY_DIR}/data"
	# COMMAND ${CMAKE_COMMAND} -E file REMOVE_RECURSE "${CMAKE_BINARY_DIR}/data"
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

file(GLOB_RECURSE RESOURCES_FLAC "${CMAKE_SOURCE_DIR}/data/audio/*.flac")

# audio output format
if(NOT FFMPEG)
	message(WARNING "FFmpeg not found: converting audio not supported\n")
	add_custom_command(
		TARGET ${EXECUTABLE} POST_BUILD
		# FIXME: should only copy .flac files
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/audio" "${CMAKE_BINARY_DIR}/data/audio"
		COMMENT "Copying audio data"
	)
else()
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
			message(FATAL_ERROR "Unsupported audio format: ${AUDIO_FORMAT}\nValid formats: ${VALID_AFMTS}")
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

if(WIN32 AND USE_BUNDLED)
	file(GLOB RESOURCES_DLL "${BUNDLED_LIBPREFIX}/bin/*.dll")
	add_custom_command(
		TARGET ${EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCES_DLL} "${CMAKE_BINARY_DIR}"
		# FIXME: this should be done when libraries are built
		COMMAND strip ${RESOURCES_DLL}
		COMMENT "Copying DLLs"
	)
	# unused DLLs
	add_custom_command(
		TARGET ${EXECUTABLE} POST_BUILD
		# FIXME: need platform-independent & case-insensitive method
		COMMAND rm -vf "wx*_gl_*.dll" "libasprintf*.dll" "libgettext*.dll"
		# COMMAND ${CMAKE_COMMAND} -E file REMOVE "wx*_gl_*.dll" "libasprintf*.dll" "libgettext*.dll"
		COMMENT "Deleting unused DLLs"
	)
endif()

# platform-dependent files preparation

# X11
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/x11")
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
	# COMMAND ${CMAKE_COMMAND} -E file REMOVE_RECURSE "${CMAKE_BINARY_DIR}/stage"
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

# regular installation
if(PORTABLE)
	set(DIR_BIN "${CMAKE_INSTALL_PREFIX}/myabcs")
	set(DIR_DATA "${CMAKE_INSTALL_PREFIX}/myabcs")
else()
	set(DIR_BIN "${CMAKE_INSTALL_PREFIX}/bin")
	set(DIR_DATA "${CMAKE_INSTALL_PREFIX}/share/myabcs")
endif()

install(
	TARGETS ${EXECUTABLE}
	RUNTIME DESTINATION "${DIR_BIN}"
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

if(NOT PORTABLE)
	install(
		FILES "${CMAKE_BINARY_DIR}/x11/myabcs.svg"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps"
	)
	install(
		FILES "${CMAKE_BINARY_DIR}/x11/myabcs.desktop"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications"
	)
endif()

add_custom_target(uninstall
	# FIXME: need platform-independent method for removing files & directories
	# COMMAND ${CMAKE_COMMAND} -E file REMOVE
	COMMAND rm -vf
		"${CMAKE_INSTALL_PREFIX}/share/applications/myabcs.desktop"
		"${CMAKE_INSTALL_PREFIX}/share/pixmaps/myabcs.svg"
		"${CMAKE_INSTALL_PREFIX}/bin/${EXECUTABLE}"
	COMMAND rm -vrf "${CMAKE_INSTALL_PREFIX}/share/myabcs"
	# COMMAND ${CMAKE_COMMAND} -E file REMOVE_RECURSE "${CMAKE_INSTALL_PREFIX}/share/myabcs"
)

# cleanup
add_custom_target(distclean
	COMMAND ${CMAKE_MAKE_PROGRAM} clean
	# FIXME: need platform-independent & case-insensitive method
	COMMAND rm -vf "*.dll"
	COMMAND rm -vrf "${CMAKE_BINARY_DIR}/data"
	# COMMAND ${CMAKE_COMMAND} -E file REMOVE "*.dll"
	# COMMAND ${CMAKE_COMMAND} -E file REMOVE_RECURSE "${CMAKE_BINARY_DIR}/data"
)
