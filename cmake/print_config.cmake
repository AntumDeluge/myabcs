
set(os_name "unknown")
if(CMAKE_HOST_SYSTEM_NAME)
	set(os_name "${CMAKE_HOST_SYSTEM_NAME}")
	if(CMAKE_CXX_PLATFORM_ID)
		set(os_name "${os_name} (${CMAKE_CXX_PLATFORM_ID})")
	endif()
endif()

set(info_compiler "${CMAKE_CXX_COMPILER_ID}")
if (CMAKE_CXX_COMPILER_VERSION)
	set(info_compiler "${info_compiler} (${CMAKE_CXX_COMPILER_VERSION})")
endif()

set(target_arch "unknown")
if(ARCH)
	set(target_arch "${ARCH}")
endif()

set(info_pthreads "no")
# this should always be defined
if(CMAKE_USE_PTHREADS_INIT)
	set(info_pthreads "yes")
endif()

set(info_ffmpeg "no")
if(FFMPEG)
	set(info_ffmpeg "yes")
endif()

set(info_aformat "${AUDIO_FORMAT}")
if(AUDIO_FORMAT STREQUAL "copy" OR AUDIO_FORMAT STREQUAL "flac")
	set(info_aformat "copy (flac)")
endif()

set(info_portable "yes")
if(NOT PORTABLE)
	set(info_portable "no (data root: ${DATAROOT})")
endif()

set(info_static "no")
if(STATIC)
	set(info_static "yes")
endif()

message("\n\n  *** Host System ***\n")
message(STATUS "Name: ................ ${os_name}")
message(STATUS "Version: ............. ${CMAKE_HOST_SYSTEM_VERSION}")
message(STATUS "Architecture: ........ ${CMAKE_HOST_SYSTEM_PROCESSOR}")

message("\n\n  *** General CMake Options ***\n")
message(STATUS "Generator: ........... ${CMAKE_GENERATOR}")
message(STATUS "Make program ......... ${CMAKE_MAKE_PROGRAM}")
message(STATUS "Install prefix: ...... ${CMAKE_INSTALL_PREFIX}")

message("\n\n  *** Target Build ***\n")
message(STATUS "Type: ................ ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ........ ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler ID: ..... ${info_compiler}")
message(STATUS "C++ standard: ........ ${CMAKE_CXX_STANDARD}")
message(STATUS "Architecture: ........ ${target_arch}")
message(STATUS "Link statically ...... ${info_static}")

message("\n\n  *** Dependencies ***\n")
message(STATUS "wxWidgets version: ... ${wxWidgets_VERSION_STRING}")
message(STATUS "wxSVG version: ....... ${WXSVG_VERSION}")
message(STATUS "SDL2 version: ........ ${SDL2_VERSION}")
message(STATUS "SDL2_mixer version: .. ${SDL2MIXER_VERSION}")
message(STATUS "Pthreads: ............ ${info_pthreads}")
message(STATUS "FFmpeg available: .... ${info_ffmpeg} (build only)")

message("\n\n  *** App Options ***\n")
message(STATUS "Portable: ............ ${info_portable}")
message(STATUS "Audio format: ........ ${info_aformat}")

message("\nTo see all configuration options execute: cmake -LA path/to/source\n")
