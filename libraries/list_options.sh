#!/usr/bin/env bash

SCRIPT_NAME="$(basename $0)"
ROOT_DIR="$(dirname $0)/../"
cd "${ROOT_DIR}"
ROOT_DIR="$(pwd)"
LIBS_DIR="${ROOT_DIR}/libraries"
cd "${LIBS_DIR}"

AVAIL_SRCS=$(find "${LIBS_DIR}/source/" -maxdepth 1 -type d -exec echo  {} \; | sed -e "s|${LIBS_DIR}/source/||")
function show_available_libs {
	if test ${#AVAIL_SRCS[@]} -eq 0; then
		echo -e "\nNo available sources to list"
	else
		echo -e "\nAvailable libraries:\n\n"
		for AS in ${AVAIL_SRCS[@]}; do
			echo "  ${AS}"
		done
	fi
}

function show_lib_options {
	local target_dir=$1
	if test -z "${target_dir}"; then
		echo -e "\nERROR: Target directory not specified in show_lib_options function"
		exit 1
	fi
	local lib_name=$(basename "${target_dir}")
	cd "${target_dir}"
	if test -f "configure"; then
		echo -e "\n${lib_name}: Found GNU Autotools configure script"
		./configure --help
	elif test -f "CMakeLists.txt"; then
		echo -e "\n${lib_name}: Found CMake configuration"
		tmp_dir="tmp-0000000001"
		mkdir "${tmp_dir}"
		cd "${tmp_dir}"
		cmake ../ > /dev/null 2>&1
		cmake -LA ../ | awk '{if(f)print} /-- Cache values/{f=1}'
		cd ../
		rm -r "${tmp_dir}"
	elif test -f "meson.build"; then
		if test ! -f "meson_options.txt"; then
			echo -e "\n${lib_name}: ERROR: Found Meson Build configuration, but \"meson_options.txt\" is missing"
			exit 1
		fi
		echo -e "\n${lib_name}: Found Meson Build configuration"
		cat "meson_options.txt"
	else
		echo -e "\nERROR: No recognizable configuration found in ${target_dir}"
		exit 1
	fi

	cd "${LIBS_DIR}"
}

LIB_NAME=$1
if test -z "${LIB_NAME}"; then
	echo -e "\nPlease supply library directory name (usually as <name>-<version>): ${SCRIPT_NAME} <directory>"
	show_available_libs
	exit 1
fi

TARGET_DIR="${LIBS_DIR}/source/${LIB_NAME}"
if test ! -d "${TARGET_DIR}"; then
	echo -e "\nDid not find library directory: ${TARGET_DIR}"
	show_available_libs
	exit 1
fi

show_lib_options "${TARGET_DIR}"
