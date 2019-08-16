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

POSSIBLE_MATCHES=()
function show_possible_matches {
	if test ${#POSSIBLE_MATCHES[@]} -eq 0; then
		echo -e "\nDid not find any possible matches"
	else
		echo -e "\nPossible matches:\n\n"
		for PM in ${POSSIBLE_MATCHES[@]}; do
			echo "  ${PM}"
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

if test ! -z $2; then
	LIB_NAME=$1
	LIB_VER=$2
else
	IFS='-' read -r -a IN <<< "$1"
	LIB_NAME=${IN[0]}
	if test ${#IN[@]} -gt 1; then
		LIB_VER=${IN[1]}
	fi
fi

if test -z "${LIB_NAME}"; then
	echo -e "\nPlease supply library directory name (usually as <name>-<version>): ${SCRIPT_NAME} <directory>"
	show_available_libs
	exit 1
fi

if test ! -z "${LIB_VER}"; then
	FULL_NAME="${LIB_NAME}-${LIB_VER}"
else
	FULL_NAME="${LIB_NAME}"
fi

EXACT_MATCH=
for DIR in ${AVAIL_SRCS}; do
	if test "${DIR}" == "${FULL_NAME}"; then
		EXACT_MATCH="${DIR}"
		break
	fi
	IFS='-' read -r -a SRC <<< "${DIR}"
	# FIXME: use regular exparession
	if test "${SRC[0]}" == "${LIB_NAME}"; then
		POSSIBLE_MATCHES+=("${DIR}")
	fi
done

if test -z "${EXACT_MATCH}" && test ${#POSSIBLE_MATCHES[@]} -eq 0; then
	TARGET_DIR="${LIBS_DIR}/source/${FULL_NAME}"
	echo -e "\nDid not find library directory: ${TARGET_DIR}"
	show_available_libs
	exit 1
elif test -z "${EXACT_MATCH}" && test ${#POSSIBLE_MATCHES[@]} -eq 1; then
	EXACT_MATCH="${POSSIBLE_MATCHES[0]}"
	echo -e "\nFound single possible match: ${EXACT_MATCH}"
fi

if test ! -z "${EXACT_MATCH}"; then
	TARGET_DIR="${LIBS_DIR}/source/${EXACT_MATCH}"
	show_lib_options "${TARGET_DIR}"
else
	show_possible_matches
fi
