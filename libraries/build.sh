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
DIR_SRC="${DIR_LIBS}/source"
INSTALL_PREFIX="${DIR_LIBS}/libprefix-${BUILD}"

# add install prefix to PATH
export PATH="${INSTALL_PREFIX}/bin:${PATH}"

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
	cd "${DIR_SRC}"

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
		local ret=$?
	else
		${WGET} -O "${FNAME}" "${dl}"
		local ret=$?
	fi

	if test ${ret} -ne 0; then
		echo -e "\nAn error occurred while downloading file: ${dl}"
		# remove empty file created by wget
		if test -f "${FNAME}"; then
			rm "${FNAME}"
		fi
		exit ${ret}
	fi

	cd "${DIR_LIBS}"
	return ${ret}
}

# only call this function from 'extract_archive'
function extract_tarball {
	if test ${TAR_FOUND} -ne 0; then
		echo -e "\nError in function extract_tarball: 'tar' command not found"
		exit 1
	fi

	local archive=$1

	echo "Extracting tarball: ${archive}"
	${TAR} -xf "${archive}"
	return $?
}

# only call this function from 'extract_archive'
function extract_zip {
	if test ${UNZIP_FOUND} -ne 0; then
		echo -e "\nError in function extract_zip: 'unzip' command not found"
		exit 1
	fi

	local archive=$1

	echo "Extracting zip: ${archive}"
	${UNZIP} -qqo "${archive}"
	return $?
}

function extract_archive {
	local archive=$1
	if test -z "${archive}"; then
		echo -e "\nERROR in function extract_archive: 'archive' argument not given"
		exit 1
	fi

	if test ! -f "${archive}"; then
		echo -e "\nERROR in function extract_archive: file does not exist: ${archive}"
		exit 1
	fi

	echo "${archive}" | grep -q ".zip$"
	if test $? -eq 0; then
		extract_zip "${archive}"
		local ret=$?
	else
		extract_tarball "${archive}"
		local ret=$?
	fi

	return ${ret}
}


# library names in build order
BUILTIN_LIBS="pkg-config zlib libpng libiconv gettext libffi libexif expat libogg libvorbis libflac SDL2 libmpg123 SDL2_mixer wxWidgets wxSVG"

for NAME in ${BUILTIN_LIBS}; do
	echo -e "\nProcessing ${NAME} ..."

	CFG="${DIR_LIBS}/CONFIG-${NAME}"
	if test ! -f "${CFG}"; then
		echo -e "\nERROR: configuration not found: ${CFG}"
		exit 1
	fi

	# unset values imported from configuration
	VER=
	DNAME=
	FNAME=
	SOURCE=
	CONFIG_OPTS=
	EXTRACT_NAME=
	CMD_CONFIG=
	CMD_BUILD=
	CMD_INSTALL=
	CPPFLAGS="-I${INSTALL_PREFIX}/include"
	LDFLAGS="-L${INSTALL_PREFIX}/lib"
	AUTORECONF=false

	# import configuration
	. "${CFG}"

	# check values imported from configuration
	if test -z ${VER} || test -z ${DNAME} || test -z ${FNAME} || test -z ${SOURCE}; then
		echo -e "\nERROR: malformed configuration: ${CFG}"
		exit 1
	fi

	export CPPFLAGS LDFLAGS

	EXTRACT_DONE=false
	CONFIG_DONE=false
	BUILD_DONE=false
	INSTALL_DONE=false

	LIB_BUILD="${NAME}-${VER}-${BUILD}"

	# prevents re-build/re-install
	FILE_LIB_INSTALL="${DIR_LIBS}/INSTALL-${LIB_BUILD}"
	if test -f "${FILE_LIB_INSTALL}"; then
		. "${FILE_LIB_INSTALL}"
	fi

	if ${INSTALL_DONE}; then
		echo "Using previous install of ${NAME} ${VER}"
	else
		if ${EXTRACT_DONE}; then
			echo "Not re-extracting ${FNAME}"
		else
			PACKAGE="${DIR_SRC}/${FNAME}"
			if test -f "${PACKAGE}"; then
				echo "Found package: ${FNAME}"
			else
				download_source "${SOURCE}" "${FNAME}"
			fi

			cd "${DIR_SRC}"
			extract_archive "${PACKAGE}"
			if test $? -ne 0; then
				echo -e "\nAn error occurred while extracting file: ${PACKAGE}"
				exit 1
			fi

			# use standard naming convention
			if test ! -z "${EXTRACT_NAME}"; then
				if test -d "${DNAME}"; then
					rm -rf "${DNAME}"
				fi
				mv "${EXTRACT_NAME}" "${DNAME}"
			fi

			cd "${DNAME}"

			# apply patches
			if test -d "../../patch/"; then
				PATCHES=$(ls "../../patch/" | grep "^${NAME}-.*\.patch")
			fi

			for PATCH in ${PATCHES}; do
				echo "Applying patch: ${PATCH}"
				patch -p1 -i "../../patch/${PATCH}"
				ret=$?
				if test ${ret} -ne 0; then
					echo -e "\nAn error occurred while trying to apply patch: ${PATCH}"
					exit ${ret}
				fi
			done

			if ${AUTORECONF}; then
				echo "Running autoreconf ..."
				autoreconf -fiv
				ret=$?
				if test ${ret} -ne 0; then
					echo -e "\nAn error occurred running autoreconf for ${NAME} ${VER}"
					exit ${ret}
				fi
			fi

			cd "${DIR_LIBS}"
			echo "EXTRACT_DONE=true" >> "${FILE_LIB_INSTALL}"
		fi

		DIR_BUILD="build/${LIB_BUILD}"
		if ${BUILD_DONE}; then
			echo "Not re-building ${NAME} ${VER}"
		else
			echo "Building ${NAME} ${VER} ..."

			if test ! -d "${DIR_BUILD}"; then
				mkdir -p "${DIR_BUILD}"
			fi

			cd "${DIR_BUILD}"

			if ${CONFIG_DONE}; then
				echo "Not re-configuring ${NAME} ${VER}"
			else
				echo "Configuring ${NAME} ${VER} ..."

				if test -z "${CMD_CONFIG}"; then
					# add common config options
					CONFIG_OPTS+=" --prefix=${INSTALL_PREFIX}"
					case "${NAME}" in
						"wxWidgets")
							CONFIG_OPTS+=" --enable-shared=no"
							;;
						"zlib")
							CONFIG_OPTS+=" --static"
							;;
						*)
							CONFIG_OPTS+=" --enable-shared=no --enable-static=yes"
							;;
					esac

					"${DIR_SRC}/${DNAME}/configure" ${CONFIG_OPTS}
					if test $? -ne 0; then
						echo -e "\nAn error occurred while configuring ${NAME} ${VER}"
						exit 1
					fi
				else
					${CMD_CONFIG}
					if test $? -ne 0; then
						echo -e "\nAn error occurred while configuring ${NAME} ${VER}"
						exit 1
					fi
				fi

				echo "CONFIG_DONE=true" >> "${FILE_LIB_INSTALL}"
			fi

			if test -z "${CMD_BUILD}"; then
				make
				if test $? -ne 0; then
					echo -e "\nAn error occurred while building ${NAME} ${VER}"
					exit 1
				fi
			else
				${CMD_BUILD}
				if test $? -ne 0; then
					echo -e "\nAn error occurred while building ${NAME} ${VER}"
					exit 1
				fi
			fi

			echo "BUILD_DONE=true" >> "${FILE_LIB_INSTALL}"

			cd "${DIR_LIBS}"
		fi

		cd "${DIR_BUILD}"
		if test $? -ne 0; then
			# build directory doesn't exist so we exit to prevent 'make install' from being called
			exit 1
		fi

		if test -z "${CMD_INSTALL}"; then
			make install
			if test $? -ne 0; then
				echo -e "\nAn error occurred while installing ${NAME} ${VER}"
				exit 1
			fi
		else
			${CMD_INSTALL}
			if test $? -ne 0; then
				echo -e "\nAn error occurred while installing ${NAME} ${VER}"
				exit 1
			fi
		fi

		echo "INSTALL_DONE=true" >> "${FILE_LIB_INSTALL}"

		cd "${DIR_LIBS}"
	fi
done
