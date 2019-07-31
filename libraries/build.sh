#!/bin/sh

# script to build "built-in" library dependencies


BUILD=$1

if test -z "${BUILD}"; then
	echo -e "\nERROR: please provide \"BUILD\" argument: build.sh BUILD"
	exit 1
fi

echo -e "\nBuilding libraries for: ${BUILD}"

DIR_LIBS="`dirname $0`"
cd "${DIR_LIBS}"
DIR_LIBS="`pwd`" # get full directory path
DIR_ROOT="`dirname ${DIR_LIBS}`"
INSTALL_PREFIX="${DIR_LIBS}/libprefix-${BUILD}"

# check for Windows OS & detect MinGW build architecture
OS_WIN=false
case "${OSTYPE}" in
	"win32")
		OS_WIN=true
		;;
	"msys")
		OS_WIN=true
		;;
	"cygwin")
		OS_WIN=true
		;;
esac

TAR=`which tar`
TAR_FOUND=$?
UNZIP=`which unzip`
UNZIP_FOUND=$?
WGET=`which wget`
WGET_FOUND=$?

function download_source {
	if test ${WGET_FOUND} -ne 0; then
		echo -e "\nError in function download_source: 'wget' command not found"
		exit 1
	fi

	local dl=$1
	if test -z "${dl}"; then
		echo -e "\nERROR in function download_source: URL not given"
		exit 1
	fi

	echo -e "\nDownloading: ${dl}"
	local tname=$2
	if test -z "${tname}"; then
		${WGET} "${dl}"
	else
		${WGET} -O "${FNAME}" "${dl}"
	fi

	if test $? -ne 0; then
		echo -e "\nAn error occurred during download of file: ${dl}"
		# remove empty file created by wget
		if test -f "${FNAME}"; then
			rm "${FNAME}"
		fi
		exit 1
	fi
}

function extract_tarball {
	if test ${TAR_FOUND} -ne 0; then
		echo -e "\nError in function extract_tarball: 'tar' command not found"
		exit 1
	fi

	local archive=$1
	if test -z "${archive}"; then
		echo -e "\nERROR in function extract_tarball: 'archive' argument not given"
		exit 1
	fi

	if test ! -f "${archive}"; then
		echo -e "\nERROR in function extract_tarball: file does not exist: ${archive}"
		exit 1
	fi

	echo "Extracting tarball: ${archive}"
	${TAR} -xf "${archive}"
	return $?
}

function extract_zip {
	if test ${UNZIP_FOUND} -ne 0; then
		echo -e "\nError in function extract_zip: 'unzip' command not found"
		exit 1
	fi

	local archive=$1
	if test -z "${archive}"; then
		echo -e "\nERROR in function extract_zip: 'archive' argument not given"
		exit 1
	fi

	if test ! -f "${archive}"; then
		echo -e "\nERROR in function extract_zip: file does not exist: ${archive}"
		exit 1
	fi

	echo "Extracting zip: ${archive}"
	${UNZIP} -qqo "${archive}"
	return $?
}

# library names in build order
LIB_NAMES="libogg libvorbis SDL2 SDL2_mixer wxWidgets wxSVG"

for NAME in ${LIB_NAMES}; do
	echo -e "\nProcessing ${NAME} ..."

	CFG="${DIR_LIBS}/CONFIG-${NAME}"
	if test ! -f "${CFG}"; then
		echo -e "\nERROR: configuration not found: ${CFG}"
		exit 1
	fi

	# unset values imported from configuration
	VER=
	FNAME=
	SOURCE=

	# import configuration
	. "${CFG}"

	# check values imported from configuration
	if test -z ${VER} || test -z ${FNAME} || test -z ${SOURCE}; then
		echo -e "\nERROR: malformed configuration: ${CFG}"
		exit 1
	fi

	EXTRACT_DONE=false
	CONFIG_DONE=false
	INSTALL_DONE=false

	LIB_BUILD="${NAME}-${VER}-${BUILD}"

	# prevents re-build/re-install
	FILE_LIB_INSTALL="${DIR_LIBS}/INSTALL-${LIB_BUILD}"
	if test -f "${FILE_LIB_INSTALL}"; then
		. "${FILE_LIB_INSTALL}"
	fi

	PACKAGE="${DIR_LIBS}/${FNAME}"
	if test -f "${PACKAGE}"; then
		echo "Found package: ${FNAME}"
	else
		download_source "${SOURCE}" "${FNAME}"
	fi

	if ${EXTRACT_DONE}; then
		echo "Not re-extracting ${FNAME}"
	else
		echo "${FNAME}" | grep -q ".zip$"
		if test $? -eq 0; then
			extract_zip "${PACKAGE}"
		else
			extract_tarball "${PACKAGE}"
		fi

		if test $? -ne 0; then
			echo -e "\nAn error occurred while extracting file: ${PACKAGE}"
			exit 1
		fi

		echo "EXTRACT_DONE=true" >> "${FILE_LIB_INSTALL}"
	fi
done
