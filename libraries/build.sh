#!/usr/bin/env bash

# script to build "built-in" library dependencies


BUILD=$1
EXTRACT_ONLY=false
if test ! -z $2; then
	if test "$2" == "extract"; then
		EXTRACT_ONLY=true
		BUILD_LIBS="${@:3}"
	else
		BUILD_LIBS="${@:2}"
	fi
fi

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
if test ! ${TAR_FOUND} -eq 0; then
	TAR=`which bsdtar`
	TAR_FOUND=$?
fi
UNZIP=`which unzip`
UNZIP_FOUND=$?
WGET=`which wget`
WGET_FOUND=$?

function download_source {
	if test ! -d "${DIR_SRC}"; then
		mkdir -p "${DIR_SRC}"
	fi
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
	local exclude=$2

	echo "Extracting tarball: ${archive}"
	if test -z "${exclude}"; then
		${TAR} -xf "${archive}"
	else
		${TAR} --exclude "${exclude}" -xf "${archive}"
	fi
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
	local exclude=$2
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
		extract_zip "${archive}" "${exclude}"
		local ret=$?
	else
		extract_tarball "${archive}" "${exclude}"
		local ret=$?
	fi

	return ${ret}
}


if test ! -z "${BUILD_LIBS}"; then
	BUILTIN_LIBS="${BUILD_LIBS}"
else
	# library names in build order
	BUILTIN_LIBS="pkg-config libiconv gettext bzip2 zlib libpng xz libxml2 libjpeg-turbo libtiff \
lcms2 libffi glib libexif expat graphite2 harfbuzz freetype fontconfig pcre pixman libspectre \
poppler librsvg cairo pango libogg libvorbis libflac SDL2 libmpg123 SDL2_mixer wxWidgets wxSVG"
fi

if test ! -z "${NO_BUILD_LIBS}"; then
	for L in ${NO_BUILD_LIBS}; do
		BUILTIN_LIBS=$(echo "${BUILTIN_LIBS}" | sed -e "s|${L}||g")
	done
fi

# verbose make
export VERBOSE=1

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
	CMD_BUILD=(make)
	CMD_INSTALL=(make install)
	CPPFLAGS=
	LDFLAGS=
	LIBS=
	PRE_CONF=
	EXCLUDE_EXTRACT=

	# import configuration
	. "${CFG}"

	# check values imported from configuration
	if test -z ${VER} || test -z ${DNAME} || test -z ${FNAME} || test -z ${SOURCE}; then
		echo -e "\nERROR: malformed configuration: ${CFG}"
		exit 1
	fi

	CPPFLAGS="-I${INSTALL_PREFIX}/include ${CPPFLAGS}"
	LDFLAGS="-L${INSTALL_PREFIX}/lib ${LDFLAGS}"
	export CPPFLAGS LDFLAGS LIBS

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
			if ${EXTRACT_ONLY}; then
				continue
			fi
		else
			PACKAGE="${DIR_SRC}/${FNAME}"
			if test -f "${PACKAGE}"; then
				echo "Found package: ${FNAME}"
			else
				download_source "${SOURCE}" "${FNAME}"
			fi

			cd "${DIR_SRC}"

			# clean up old files
			if test -d "${DNAME}"; then
				rm -rf "${DNAME}"
			fi

			extract_archive "${PACKAGE}" "${EXCLUDE_EXTRACT}"
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
				PATCHES=$(ls "${DIR_LIBS}/patch/" | grep "^${NAME}-.*\.patch")
			fi

			for PATCH in ${PATCHES}; do
				if ${OS_WIN}; then
					P_SYS="win32"
				else
					P_SYS="${OSTYPE}"
				fi

				P_TYPE=$(basename ${PATCH} | sed -e "s|^${NAME}-||" | cut -d"-" -f2)
				case "${P_TYPE}" in
					"any"|"all"|"${P_SYS}")
						echo "Applying patch: ${PATCH}"
						patch -p1 -i "${DIR_LIBS}/patch/${PATCH}"
						ret=$?
						if test ${ret} -ne 0; then
							echo -e "\nAn error occurred while trying to apply patch: ${PATCH}"
							exit ${ret}
						fi
						;;
					*)
						echo "Ignoring patch for non-${P_SYS} system: ${PATCH}"
						;;
					esac
			done

			if test ! -z ${PRE_CONF}; then
				echo "Preparing source ..."
				"${PRE_CONF[@]}"
				ret=$?
				if test ${ret} -ne 0; then
					echo -e "\nAn error occurred preparing source for ${NAME} ${VER}"
					exit ${ret}
				fi
			fi

			cd "${DIR_LIBS}"
			echo "EXTRACT_DONE=true" >> "${FILE_LIB_INSTALL}"
			if ${EXTRACT_ONLY}; then
				continue
			fi
		fi

		DIR_BUILD="build/${LIB_BUILD}"
		if ${BUILD_DONE}; then
			echo "Not re-building ${NAME} ${VER}"
		else
			if test ! -d "${DIR_BUILD}"; then
				mkdir -p "${DIR_BUILD}"
			fi

			cd "${DIR_BUILD}"

			if ${CONFIG_DONE}; then
				echo "Not re-configuring ${NAME} ${VER}"
			else
				echo "Configuring ${NAME} ${VER} ..."

				# remove old cache if CONFIG_DONE == false
				find ./ -type f -delete
				find ./ -mindepth 1 -type d -empty -delete

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
					# common values for CMake
					echo "${CMD_CONFIG[0]}" | grep -q "cmake$"
					if test $? -eq 0; then
						if test "${OSTYPE}" == "msys"; then
							CMD_CONFIG+=(-G "MSYS Makefiles")
						fi
						CMD_CONFIG+=(-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} -DCMAKE_PREFIX_PATH=${INSTALL_PREFIX})
						CMD_CONFIG+=(-DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DENABLE_STATIC=ON -DENABLE_SHARED=OFF)
						CMD_CONFIG+=(-DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_FIND_LIBRARY_SUFFIXES=".a" "${DIR_SRC}/${DNAME}")
					fi

					# common values for meson
					echo "${CMD_CONFIG[0]}" | grep -q "meson$"
					if test $? -eq 0; then
						CMD_CONFIG+=(--prefix=${INSTALL_PREFIX} --buildtype=plain --default-library=static "${DIR_SRC}/${DNAME}")
					fi

					"${CMD_CONFIG[@]}"
					if test $? -ne 0; then
						echo -e "\nAn error occurred while configuring ${NAME} ${VER}"
						exit 1
					fi
				fi

				echo "CONFIG_DONE=true" >> "${FILE_LIB_INSTALL}"
			fi

			echo "Building ${NAME} ${VER} ..."

			"${CMD_BUILD[@]}"
			if test $? -ne 0; then
				echo -e "\nAn error occurred while building ${NAME} ${VER}"
				exit 1
			fi

			echo "BUILD_DONE=true" >> "${FILE_LIB_INSTALL}"

			cd "${DIR_LIBS}"
		fi

		cd "${DIR_BUILD}"
		if test $? -ne 0; then
			# build directory doesn't exist so we exit to prevent 'make install' from being called
			exit 1
		fi

		"${CMD_INSTALL[@]}"
		if test $? -ne 0; then
			echo -e "\nAn error occurred while installing ${NAME} ${VER}"
			exit 1
		fi

		echo "INSTALL_DONE=true" >> "${FILE_LIB_INSTALL}"
		echo -e "\nFinished processing ${NAME} ${VER}"

		cd "${DIR_LIBS}"
	fi
done
