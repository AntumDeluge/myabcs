#!/usr/bin/env bash

SCRIPT_NAME="$(basename $0)"
ROOT_DIR="$(dirname $0)/../"
cd "${ROOT_DIR}"
ROOT_DIR="$(pwd)"
LIBS_DIR="${ROOT_DIR}/libraries"
cd "${LIBS_DIR}"

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

AVAIL_CONFIGS=$(find "${LIBS_DIR}/" -mindepth 1 -maxdepth 1 -type f -name "CONFIG-*")
if test -d "${LIBS_DIR}/CONFIG/"; then
	AVAIL_CONFIGS+=$(find "${LIBS_DIR}/CONFIG/" -mindepth 1 -maxdepth 1 -type f)
fi
AVAIL_CONFIGS=$(echo "${AVAIL_CONFIGS}" | sed -e "s|${LIBS_DIR}/CONFIG/||g" -e "s|${LIBS_DIR}/||g" -e 's|^CONFIG-||g')

function show_available_configs {
	if test ${#AVAIL_CONFIGS[@]} -eq 0; then
		echo -e "\nNo available configurations to list"
	else
		echo -e "\nAvailable configurations:\n\n"
		for CFG in ${AVAIL_CONFIGS}; do
			echo "  ${CFG}"
		done
	fi
}

FOUND_CONFIG=false
CONFIG_PATH=
function is_config_available {
	local libname=$1
	if test -z "${libname}"; then
		echo -e "\nERROR: lib name not specified in is_config_available function"
		return 1
	fi

	for CFG in ${AVAIL_CONFIGS}; do
		if test "${libname}" == "${CFG}"; then
			# get path to configuration file
			if test -f "${LIBS_DIR}/CONFIG-${libname}"; then
				CONFIG_PATH="${LIBS_DIR}/CONFIG-${libname}"
			else
				CONFIG_PATH="${LIBS_DIR}/CONFIG/${libname}"
			fi

			return 0
		fi
	done

	return 1
}

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

	local NAME="${LIB_NAME}"
	local DIR_SRC="${LIBS_DIR}/source"
	DIR_CONFIG_ROOT="${target_dir}"

	if ${FOUND_CONFIG}; then
		# check for custom config root
		. "${CONFIG_PATH}"
	fi

	cd "${target_dir}"

	if test -f "${DIR_CONFIG_ROOT}/configure"; then
		echo -e "\n${NAME}: Found GNU Autotools configure script"
		"${DIR_CONFIG_ROOT}/configure" --help
	elif test -f "${DIR_CONFIG_ROOT}/CMakeLists.txt"; then
		echo -e "\n${NAME}: Found CMake configuration"
		tmp_dir="tmp-0000000001"
		mkdir "${tmp_dir}"
		cd "${tmp_dir}"
		cmake ${CONFIG_OPTS[@]} ${LIBTYPE_OPTS[@]} "${DIR_CONFIG_ROOT}" > /dev/null 2>&1
		cmake -LA  ${CONFIG_OPTS[@]} ${LIBTYPE_OPTS[@]} "${DIR_CONFIG_ROOT}" | awk '{if(f)print} /-- Cache values/{f=1}'
		cd ../
		rm -r "${tmp_dir}"
	elif test -f "${DIR_CONFIG_ROOT}/meson.build"; then
		if test ! -f "${DIR_CONFIG_ROOT}/meson_options.txt"; then
			echo -e "\n${NAME}: ERROR: Found Meson Build configuration, but \"meson_options.txt\" is missing"
			exit 1
		fi
		echo -e "\n${NAME}: Found Meson Build configuration"
		cat "${DIR_CONFIG_ROOT}/meson_options.txt"
	else
		echo -e "\nERROR: No recognizable configuration found\nTry deleting the directory \"${target_dir}\" & run again"
		exit 1
	fi

	cd "${LIBS_DIR}"
}

is_config_available "${LIB_NAME}"
if test $? -eq 0; then
	FOUND_CONFIG=true
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
	if ! ${FOUND_CONFIG}; then
		TARGET_DIR="${LIBS_DIR}/source/${FULL_NAME}"
		echo -e "\nDid not find library directory: ${TARGET_DIR}"
		show_available_libs
		exit 1
	else
		echo -e "\nFound config for ${LIB_NAME}"
		cd "${ROOT_DIR}"
		make build-libs BUILD_LIBS="extract ${LIB_NAME}"
		cd "${LIBS_DIR}"
		echo -e "\nNow re-run \`./${SCRIPT_NAME} ${LIB_NAME}\`"
		exit 0
	fi
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
