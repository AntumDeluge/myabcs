#!/usr/bin/env bash

# script to build "built-in" library dependencies


BUILD=$1
PREPARE_ONLY=false
if test ! -z $2; then
	if test "$2" == "extract"; then
		PREPARE_ONLY=true
		BUILD_LIBS="${@:3}"
	else
		BUILD_LIBS="${@:2}"
	fi
fi

if test -z "${BUILD}"; then
	echo -e "\nERROR: please provide \"BUILD\" argument: build.sh BUILD"
	exit 1
fi

if ! ${PREPARE_ONLY}; then
	echo -e "\nBuilding libraries for: ${BUILD}"
fi

DIR_LIBS="`dirname $0`"
cd "${DIR_LIBS}"
DIR_LIBS="`pwd`" # get full directory path
DIR_ROOT="`dirname ${DIR_LIBS}`"
DIR_SRC="${DIR_LIBS}/source"
DIR_BUILD="${DIR_LIBS}/build"
INSTALL_PREFIX="${DIR_LIBS}/libprefix-${BUILD}"

STATIC=true
CLEAN_LIBTOOL=false

if test ! -d "${DIR_SRC}"; then
	mkdir -p "${DIR_SRC}"
fi
if test ! -d "${DIR_BUILD}"; then
	mkdir -p "${DIR_BUILD}"
fi

# add install prefix to PATH
export PATH="${INSTALL_PREFIX}/bin:${PATH}"
export PKG_CONFIG_PATH="${INSTALL_PREFIX}/lib/pkgconfig:${INSTALL_PREFIX}/share/pkgconfig:${PKG_CONFIG_PATH}"

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

# A simple function to test if a variable is an array
#
# NOTE: parameter arguments must be enclosed in quotes
function is_array {
	# FIXME: this is NOT the best way to do this
	local count=$#

	if test ${count} -gt 1; then
		return 0
	fi

	return 1
}

function download_source {
	if test ${WGET_FOUND} -ne 0; then
		echo -e "\nError in function download_source: 'wget' command not found"
		exit 1
	fi

	local cmd_dl=(${WGET})
	local dl=$1
	if test -z "${dl}"; then
		echo -e "\nERROR in function download_source: URL not given"
		exit 1
	fi

	# check for FTP URL
# 	grep "^ftp://" <<< "${dl}" > /dev/null 2>&1
# 	if test $? -eq 0; then
# 		cmd_dl+=("--no-passive-ftp")
# 	fi

	local tname=$2
	if test ! -z "${tname}"; then
		cmd_dl+=("-O" "${FNAME}")
	fi

	cmd_dl+=("${dl}")

	echo -e "\nDownloading: ${dl}"
	"${cmd_dl[@]}"
	local ret=$?

	if test ${ret} -ne 0; then
		echo -e "\nAn error occurred while downloading file: ${dl}"
		# remove empty file created by wget
		if test -f "${FNAME}"; then
			rm "${FNAME}"
		fi
		exit ${ret}
	fi

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


prepare() (
	local NAME="$1"
	if test -z "${NAME}"; then
		echo -e "\nError in prepare function, NAME not set"
		exit 1
	fi

	local CFG="${DIR_LIBS}/CONFIG/${NAME}"
	if test ! -f "${CFG}"; then
		local CFG="${DIR_LIBS}/CONFIG-${NAME}"
	fi

	if test ! -f "${CFG}"; then
		echo -e "\nERROR: configuration not found:\n  ${DIR_LIBS}/CONFIG/${NAME}: not found\n  ${CFG}: not found"
		exit 1
	fi

	# reset main values
	local VER DNAME FNAME EXTRACT_NAME SOURCE DEPENDS

	# only set this here for use in list_options.sh script
	local DIR_CONFIG_ROOT

	# reset options
	local PATCH_PRUNE_LEVEL CRLF_TO_LF EXCLUDE_EXTRACT

	# reset commands
	local CMD_DOWNLOAD CMD_EXTRACT

	# reset functions that can be defined in config files
	unset pre_dl post_dl pre_extract post_extract

	local REBUILD=false

	# reset status
	local DOWNLOAD_DONE=false
	local EXTRACT_DONE=false
	local PREPARE_DONE=false

	# backup original name in case of rebuild
	local NAME_ORIG="${NAME}"

	# import configuration
	. "${CFG}"

	# detect rebuild
	if test "${NAME_ORIG}" != "${NAME}"; then
		echo -e "\nDoing re-build of ${NAME} ${VER}"
		local REBUILD=true
	fi

	# check values imported from configuration
	if test -z "${VER}" || test -z "${DNAME}"; then
		echo -e "\nERROR: malformed configuration, VER & DNAME must be set: ${CFG}"
		exit 1
	fi

	# SOURCE & FNAME must be set if not using a custom download command
	if test -z "${CMD_DOWNLOAD}"; then
		if test -z "${FNAME}" || test -z "${SOURCE}"; then
			echo -e "\nERROR: malformed configuration, FNAME & SOURCE must be set: ${CFG}"
			exit 1
		fi
	fi

	# FIXME: do this in build function
	# build dependencies first
	if ! ${PREPARE_ONLY} && test ! -z "${DEPENDS}"; then
		echo -e "\nBuilding dependencies for ${NAME_ORIG} ${VER}"

		local dep_name
		for dep_name in ${DEPENDS[@]}; do
			prepare "${dep_name}"
			local ret=$?
			if test ${ret} -ne 0; then
				echo -e "\nCould not prepare ${NAME_ORIG} ${VER}"
				return ${ret}
			fi
		done
	fi

	echo -e "\nPreparing ${NAME_ORIG} ${VER}"
	echo "Using configuration: ${CFG}"

	local FILE_LIB_PREPARE="${DIR_BUILD}/PREPARE-${NAME_ORIG}-${VER}"

	if test -f "${FILE_LIB_PREPARE}"; then
		. "${FILE_LIB_PREPARE}"
	fi

	if ${REBUILD} && ! ${PREPARE_DONE}; then
		# source should already be prepared from original build
		PREPARE_DONE=true
		echo "PREPARE_DONE=true" >> "${FILE_LIB_PREPARE}"
	fi

	if ${PREPARE_DONE}; then
		echo "Using previously prepared sources for ${NAME_ORIG} ${VER}"
		if ${PREPARE_ONLY}; then
			return
		fi
	else
		cd "${DIR_SRC}"
		PACKAGE="${DIR_SRC}/${FNAME}"

		if ${DOWNLOAD_DONE}; then
			echo "Not re-downloading sources for ${NAME_ORIG} ${VER}"
		else
			# clean up old files
			if test -d "${DIR_SRC}/${DNAME}"; then
				echo "Removing old source directory: ${DIR_SRC}/${DNAME}"
				rm -rf "${DIR_SRC}/${DNAME}"
			fi

			# pre-download operations
			if test ! -z "$(type -t pre_dl)" && test "$(type -t pre_dl)" == "function"; then
				echo -e "\nRunning pre-download commands"
				pre_dl
			fi

			if test ! -z "${CMD_DOWNLOAD}"; then
				"${CMD_DOWNLOAD[@]}"
				dl_ret=$?
			else
				if test -f "${PACKAGE}"; then
					echo "Found package: ${FNAME}"
					dl_ret=0
				else
					download_source "${SOURCE}" "${FNAME}"
					dl_ret=$?
				fi
			fi

			if test ${dl_ret} -ne 0; then
				echo -e "\nAn error occurred while downloading ${NAME_ORIG} ${VER}"
				exit ${dl_ret}
			fi

			# post-download operations
			if test ! -z "$(type -t post_dl)" && test "$(type -t post_dl)" == "function"; then
				echo -e "\nRunning post-download commands"
				post_dl
			fi

			echo "DOWNLOAD_DONE=true" >> "${FILE_LIB_PREPARE}"
		fi

		if ${EXTRACT_DONE} && test -d "${DIR_SRC}/${DNAME}"; then
			echo "Not re-extracting sources for ${NAME_ORIG} ${VER}"
		else
			# clean up old files
			if test -d "${DIR_SRC}/${DNAME}"; then
				echo "Removing old source directory: ${DIR_SRC}/${DNAME}"
				rm -rf "${DIR_SRC}/${DNAME}"
			fi

			# pre-extract operations
			if test ! -z "$(type -t pre_extract)" && test "$(type -t pre_extract)" == "function"; then
				echo -e "\nRunning pre-extract commands"
				pre_extract
			fi

			if test ! -z "${CMD_EXTRACT}"; then
				"${CMD_EXTRACT[@]}"
			else
				extract_archive "${PACKAGE}" "${EXCLUDE_EXTRACT}"
			fi

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

			# FIXME: how to do this in POST_EXTRACT?
			if test ! -z "${CRLF_TO_LF}"; then
				for F in ${CRLF_TO_LF[@]}; do
					sed -i -e 's|\r$||g' "${DIR_SRC}/${DNAME}/${F}"
					if test $? -ne 0; then
						echo -e "\nAn error occurred while attempting to convert CRLF line endings to LF: ${DIR_SRC}/${DNAME}/${F}"
						exit 1
					fi
				done
			fi

			cd "${DNAME}"

			# apply external/downloaded patches
			EXT_PATCH_DIR="${DIR_LIBS}/patch/EXTERNAL-${NAME}-${VER}"
			if test -d "${EXT_PATCH_DIR}"; then
				if test -z "${PATCH_PRUNE_LEVEL}"; then
					echo -e "\nERROR: PATCH_PRUNE_LEVEL must be set when applying external patches"
					exit 1
				fi

				# FIXME: need to delete patches for older versions (probably before source download)
				for PATCH in $(find "${EXT_PATCH_DIR}" -maxdepth 1 -type f); do
					echo "Applying external patch: $(basename ${PATCH})"
					# XXX: downloaded patches may need a different "prune" level
					patch -p${PATCH_PRUNE_LEVEL} -i "${PATCH}"
					ret=$?
					if test ${ret} -ne 0; then
						echo -e "\nAn error occurred while trying to apply external patch: $(basename ${PATCH})"
						exit ${ret}
					fi
				done
			fi

			# apply internal patches
			if test -d "${DIR_LIBS}/patch/"; then
				PATCHES=$(ls "${DIR_LIBS}/patch/" | grep "^${NAME}-.*\.patch")
			fi

			for PATCH in ${PATCHES}; do
				if ${OS_WIN}; then
					local this_sys="win32"
				else
					local this_sys="${OSTYPE}"
				fi

				local p_name=$(echo ${PATCH} | sed -e "s|^${NAME}-||" -e 's|\.patch$||')
				local p_sys=$(echo ${p_name} | cut -d"-" -f2)
				local p_static_only=false
				if test "$(echo ${p_name} | cut -d"-" -f3)" == "static_only"; then
					p_static_only=true
				fi

				case "${p_sys}" in
					"any"|"all"|"${this_sys}")
						if test ${p_static_only} && ! ${STATIC}; then
							# FIXME: better method for checking if static patch should be applied?
							echo "Ignoring static patch for non-static build: ${PATCH}"
						else
							echo "Applying patch: ${PATCH}"
							patch -p1 -i "${DIR_LIBS}/patch/${PATCH}"
							ret=$?
							if test ${ret} -ne 0; then
								echo -e "\nAn error occurred while trying to apply patch: ${PATCH}"
								exit ${ret}
							fi
						fi
						;;
					*)
						echo "Ignoring patch for non-${this_sys} system: ${PATCH}"
						;;
				esac
			done

			# post-extract operations
			if test ! -z "$(type -t post_extract)" && test "$(type -t post_extract)" == "function"; then
				echo -e "\nRunning post-extract commands"
				post_extract
			fi

			# don't append redundantly to file
			if ! ${EXTRACT_DONE}; then
				echo "EXTRACT_DONE=true" >> "${FILE_LIB_PREPARE}"
			fi
		fi

		cd "${DIR_LIBS}"
		echo "PREPARE_DONE=true" >> "${FILE_LIB_PREPARE}"
		echo -e "\nFinished preparing ${NAME_ORIG} ${VER}"
	fi

	if ! ${PREPARE_ONLY}; then
		build "${NAME_ORIG}"
		local ret=$?
		if test ${ret} -ne 0; then
			echo -e "\nCould not build ${NAME_ORIG} ${VER}"
			return ${ret}
		fi
	fi
)


build() (
	local NAME="$1"
	if test -z "${NAME}"; then
		echo -e "\nError in build function, NAME not set"
		exit 1
	fi

	local CFG="${DIR_LIBS}/CONFIG/${NAME}"
	if test ! -f "${CFG}"; then
		local CFG="${DIR_LIBS}/CONFIG-${NAME}"
	fi

	if test ! -f "${CFG}"; then
		echo -e "\nERROR: configuration not found:\n  ${DIR_LIBS}/CONFIG/${NAME}: not found\n  ${CFG}: not found"
		exit 1
	fi

	# reset main values
	local VER DNAME

	# reset directories
	local DIR_CONFIG_ROOT

	# reset options
	local CFLAGS CXXFLAGS CPPFLAGS LDFLAGS LIBS CONFIG_OPTS LIBTYPE_OPTS

	# reset commands
	local CMD_CONFIG
	local CMD_BUILD=(${CMD_MAKE})
	local CMD_INSTALL=(${CMD_MAKE} install)

	# reset functions that can be defined in config files
	unset pre_cfg post_cfg pre_build post_build pre_install post_install

	# reset status
	local CONFIG_DONE=false
	local BUILD_DONE=false
	local INSTALL_DONE=false

	# backup original name in case of rebuild
	local NAME_ORIG="${NAME}"

	# import configuration
	. "${CFG}"

	is_array "${LIBS[@]}"
	if test $? -eq 0; then
		LIBS="${LIBS[@]}"
	fi

	CPPFLAGS="${CPPFLAGS[@]} -I${INSTALL_PREFIX}/include"
	LDFLAGS="${LDFLAGS[@]} -L${INSTALL_PREFIX}/lib"
	export CFLAGS CXXFLAGS CPPFLAGS LDFLAGS LIBS

	local LIB_BUILD="${NAME_ORIG}-${VER}-${BUILD}"
	local FILE_LIB_INSTALL="${DIR_BUILD}/INSTALL-${LIB_BUILD}"

	# prevents re-build/re-install
	if test -f "${FILE_LIB_INSTALL}"; then
		. "${FILE_LIB_INSTALL}"
	fi

	if ${INSTALL_DONE}; then
		echo "Using previous install of ${NAME_ORIG} ${VER}"
	else
		local DIR_LIB_BUILD="${DIR_BUILD}/${LIB_BUILD}"
		if ${BUILD_DONE}; then
			echo "Not re-building ${NAME_ORIG} ${VER}"
		else
			if test ! -d "${DIR_LIB_BUILD}"; then
				mkdir -p "${DIR_LIB_BUILD}"
			fi

			cd "${DIR_LIB_BUILD}"

			if ${CONFIG_DONE}; then
				echo "Not re-configuring ${NAME_ORIG} ${VER}"
			else
				echo -e "\nConfiguring ${NAME_ORIG} ${VER} ..."

				# remove old cache if CONFIG_DONE == false
				find ./ -type f -delete
				find ./ -mindepth 1 -type d -empty -delete

				if test -z "${DIR_CONFIG_ROOT}"; then
					DIR_CONFIG_ROOT="${DIR_SRC}/${DNAME}"
				fi

				# pre-configuration operations
				if test ! -z "$(type -t pre_cfg)" && test "$(type -t pre_cfg)" == "function"; then
					echo -e "\nRunning pre-config commands"
					pre_cfg
				fi

				if test -z "${CMD_CONFIG}"; then
					# add common config options
					CONFIG_OPTS+=(--prefix="${INSTALL_PREFIX}")

					if test "${LIBTYPE_OPTS}" != "N/A"; then
						if test -z "${LIBTYPE_OPTS}"; then
							# use defaults
							if ${STATIC}; then
								LIBTYPE_OPTS=(--enable-static=yes --enable-shared=no)
							else
								LIBTYPE_OPTS=(--enable-static=no --enable-shared=yes)
							fi
						fi
					else
						unset LIBTYPE_OPTS
					fi

					CONFIG_OPTS+=(${LIBTYPE_OPTS[@]})

					"${DIR_CONFIG_ROOT}/configure" ${CONFIG_OPTS[@]}
					if test $? -ne 0; then
						echo -e "\nAn error occurred while configuring ${NAME_ORIG} ${VER}"
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
						CMD_CONFIG+=(-DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_FIND_LIBRARY_SUFFIXES=".a")

						if test "${LIBTYPE_OPTS}" != "N/A"; then
							if test -z "${LIBTYPE_OPTS}"; then
								# use defaults
								if ${STATIC}; then
									LIBTYPE_OPTS=(-DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF)
								else
									LIBTYPE_OPTS=(-DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON)
								fi
							fi
						else
							# make sure LIBTYPE_OPTS is empty if set to "N/A"
							unset LIBTYPE_OPTS
						fi

						# add build type to config options
						if test ! -z "${LIBTYPE_OPTS}"; then
							CONFIG_OPTS+=(${LIBTYPE_OPTS[@]})
						fi

						# add config options to config command & source directory as final argument in config command
						CMD_CONFIG+=(${CONFIG_OPTS[@]} "${DIR_CONFIG_ROOT}")
					fi

					# common values for meson
					echo "${CMD_CONFIG[0]}" | grep -q "meson$"
					if test $? -eq 0; then
						if test "${LIBTYPE_OPTS}" != "N/A"; then
							if test -z "${LIBTYPE_OPTS}"; then
								# use defaults
								if ${STATIC}; then
									LIBTYPE_OPTS=(--default-library=static)
								else
									LIBTYPE_OPTS=(--default-library=shared)
								fi
							fi
						else
							# make sure to LIBTYPE_OPTS is empty if set to "N/A"
							unset LIBTYPE_OPTS
						fi

						# add default options
						CONFIG_OPTS=(--prefix=${INSTALL_PREFIX} --buildtype=plain ${LIBTYPE_OPTS[@]} ${CONFIG_OPTS[@]})
						CMD_CONFIG+=(${CONFIG_OPTS[@]} "${DIR_CONFIG_ROOT}")
					fi

					# this is usually used for source that does not use build generators like GNU Autotools or CMake
					if test "${CMD_CONFIG[0]}" == "copy"; then
						for FD in $(find "${DIR_SRC}/${DNAME}" -mindepth 1 -maxdepth 1); do
							cp -r "${FD}" ./
						done
					else
						"${CMD_CONFIG[@]}"
						if test $? -ne 0; then
							echo -e "\nAn error occurred while configuring ${NAME_ORIG} ${VER}"
							exit 1
						fi
					fi
				fi

				# post-configuration operations
				if test ! -z "$(type -t post_cfg)" && test "$(type -t post_cfg)" == "function"; then
					echo -e "\nRunning post-config commands"
					post_cfg
				fi

				echo "CONFIG_DONE=true" >> "${FILE_LIB_INSTALL}"
			fi

			echo -e "\nBuilding ${NAME_ORIG} ${VER} ..."
			# pre-build operations
			if test ! -z "$(type -t pre_build)" && test "$(type -t pre_build)" == "function"; then
				echo -e "\nRunning pre-build commands"
				pre_build
			fi

			"${CMD_BUILD[@]}"
			if test $? -ne 0; then
				echo -e "\nAn error occurred while building ${NAME_ORIG} ${VER}"
				exit 1
			fi

			# post-build operations
			if test ! -z "$(type -t post_build)" && test "$(type -t post_build)" == "function"; then
				echo -e "\nRunning post-build commands"
				post_build
			fi

			echo "BUILD_DONE=true" >> "${FILE_LIB_INSTALL}"

			cd "${DIR_LIBS}"
		fi

		cd "${DIR_LIB_BUILD}"
		if test $? -ne 0; then
			# build directory doesn't exist so we exit to prevent 'make install' from being called
			exit 1
		fi

		# TODO: strip executables & shared libs

		# pre-install operations
		if test ! -z "$(type -t pre_install)" && test "$(type -t pre_install)" == "function"; then
			echo -e "\nRunning pre-install commands"
			pre_install
		fi

		echo -e "\nInstalling ${NAME_ORIG} ${VER}"
		"${CMD_INSTALL[@]}"
		if test $? -ne 0; then
			echo -e "\nAn error occurred while installing ${NAME_ORIG} ${VER}"
			exit 1
		fi

		if ${CLEAN_LIBTOOL}; then
			local lt_files=($(find "${INSTALL_PREFIX}/lib/" -type f -name "*.la"))
			if test ${#lt_files[@]} -gt 0; then
				echo "Cleaning libtool files"
				rm -vf ${lt_files[@]}
			fi
		fi

		# post-install operations
		if test ! -z "$(type -t post_install)" && test "$(type -t post_install)" == "function"; then
			echo -e "\nRunning post-install commands"
			post_install
		fi

		echo "INSTALL_DONE=true" >> "${FILE_LIB_INSTALL}"
		echo -e "\nFinished processing ${NAME_ORIG} ${VER}"

		cd "${DIR_LIBS}"
	fi
)


# NOTES:
#
# May need to add configs for the following libs/utils
#	jsoncpp, md4c, libjasper, curl, xcb, rust
#
# The following libs/utils have trouble building
#	gettext, ncurses
#
# The following libs/utils have minimal or no dependencies
#	zlib, bzip2, expat, graphite2, libffi, libogg, xorg-util-macros,
#	gperf, libtool, nasm, pth, termcap, winpthreads, pthreads-win32

if test ! -z "${BUILD_LIBS}"; then
	BUILTIN_LIBS="${BUILD_LIBS}"
else
	# base libs in build order
	BUILTIN_LIBS=(SDL2_mixer wxSVG)
fi

if test ! -z "${NO_BUILD_LIBS}"; then
	for L in ${NO_BUILD_LIBS}; do
		BUILTIN_LIBS=$(echo "${BUILTIN_LIBS}" | sed -e "s|${L}||g")
	done
fi

# verbose make
export VERBOSE=1

if test -z "${CMD_MAKE}"; then
	CMD_MAKE="make"
fi

for NAME in ${BUILTIN_LIBS[@]}; do
	echo -e "\nProcessing ${NAME} ..."
	prepare "${NAME}"
	ret=$?
	if test ${ret} -ne 0; then
		echo -e "\nExiting with errors"
		exit ${ret}
	fi
done

# for any packages that install DLLs into lib dir
if ${OS_WIN} && test -d "${INSTALL_PREFIX}/lib/"; then
	mkdir -p "${INSTALL_PREFIX}/bin/"
	for DLL in $(find "${INSTALL_PREFIX}/lib/" -type f -name "*.[dD][lL][lL]"); do
		mv -v "${DLL}" "${INSTALL_PREFIX}/bin/"
	done
fi

# unnecessary directories
NOUSE_DIRS=("share/doc" "share/man")
for DIR in ${NOUSE_DIRS[@]}; do
	if test -d "${INSTALL_PREFIX}/${DIR}"; then
		echo -e "\nRemoving unused dir: ${DIR}"
		rm -rf "${INSTALL_PREFIX}/${DIR}"
	fi
done

# remove empty directories
find "${INSTALL_PREFIX}/" -mindepth 1 -type d -empty -delete
