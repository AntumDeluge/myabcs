#!/usr/bin/env bash

SCRIPT_NAME="$(basename $0)"
ROOT_DIR="$(dirname $0)/../"
cd "${ROOT_DIR}"
ROOT_DIR="$(pwd)"
LIBS_DIR="${ROOT_DIR}/libraries"
cd "${LIBS_DIR}"

LIB_NAME=$1
if test -z "${LIB_NAME}"; then
	echo -e "\nPlease supply library directory name (usually as <name>-<version>): ${SCRIPT_NAME} <directory>"
	exit 1
fi

TARGET_DIR="${LIBS_DIR}/source/${LIB_NAME}"
if test ! -d "${TARGET_DIR}"; then
	echo -e "\n: Did not find library directory: ${TARGET_DIR}"
	echo "Available libraries:"
	find "${LIBS_DIR}/source/" -maxdepth 1 -type d -exec echo  {} \; | sed -e "s|${LIBS_DIR}/source/||"
	exit 0
fi

cd "${TARGET_DIR}"
if test -f "configure"; then
	echo -e "\n${LIB_NAME}: Found GNU Autotools configure script"
	./configure --help
elif test -f "CMakeLists.txt"; then
	echo -e "\n${LIB_NAME}: Found CMake configuration"
	tmp_dir="tmp-0000000001"
	mkdir "${tmp_dir}"
	cd "${tmp_dir}"
	cmake ../ > /dev/null 2>&1
	cmake -LA ../ | awk '{if(f)print} /-- Cache values/{f=1}'
	cd ../
	rm -r "${tmp_dir}"
elif test -f "meson.build"; then
	if test ! -f "meson_options.txt"; then
		echo -e "\n${LIB_NAME}: ERROR: Found Meson Build configuration, but \"meson_options.txt\" is missing"
		exit 1
	fi
	echo -e "\n${LIB_NAME}: Found Meson Build configuration"
	cat "meson_options.txt"
else
	echo -e "\nERROR: No recognizable configuration found in ${TARGET_DIR}"
	exit 1
fi

cd "${LIBS_DIR}"
