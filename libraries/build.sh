#!/usr/bin/env bash

# script to build "built-in" library dependencies


build_type=$1
prepare_only=false
if test ! -z $2; then
	if test "$2" == "extract"; then
		prepare_only=true
		build_libs="${@:3}"
	else
		build_libs="${@:2}"
	fi
fi

if test -z "${build_type}"; then
	echo -e "\nERROR: please provide \"build_type\" argument: build.sh build_type"
	exit 1
fi

if ! ${prepare_only}; then
	echo -e "\nBuilding libraries for: ${build_type}"
fi

dir_libs="`dirname $0`"
cd "${dir_libs}"
dir_libs="`pwd`" # get full directory path
dir_root="`dirname ${dir_libs}`"
dir_src="${dir_libs}/source"
dir_build="${dir_libs}/build"
install_prefix="${dir_libs}/libprefix-${build_type}"

static=true
clean_libtool=false

if test ! -d "${dir_src}"; then
	mkdir -p "${dir_src}"
fi
if test ! -d "${dir_build}"; then
	mkdir -p "${dir_build}"
fi

# add install prefix to PATH
export PATH="${install_prefix}/bin:${PATH}"
export PKG_CONFIG_PATH="${install_prefix}/lib/pkgconfig:${install_prefix}/share/pkgconfig:${PKG_CONFIG_PATH}"

# check for Windows OS & detect MinGW build architecture
os_win=false
case "${OSTYPE}" in
	"win32")
		os_win=true
		;;
	"msys")
		os_win=true
		;;
	"cygwin")
		os_win=true
		;;
esac

cmd_tar=`which tar`
tar_found=$?
if test ! ${tar_found} -eq 0; then
	cmd_tar=`which bsdtar`
	tar_found=$?
fi
cmd_unzip=`which unzip`
unzip_found=$?
cmd_wget=`which wget`
wget_found=$?

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
	if test ${wget_found} -ne 0; then
		echo -e "\nError in function download_source: 'wget' command not found"
		exit 1
	fi

	local cmd_dl=(${cmd_wget})
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
		cmd_dl+=("-O" "${fname}")
	fi

	cmd_dl+=("${dl}")

	echo -e "\nDownloading: ${dl}"
	"${cmd_dl[@]}"
	local ret=$?

	if test ${ret} -ne 0; then
		echo -e "\nAn error occurred while downloading file: ${dl}"
		# remove empty file created by wget
		if test -f "${fname}"; then
			rm "${fname}"
		fi
		exit ${ret}
	fi

	return ${ret}
}

# only call this function from 'extract_archive'
function extract_tarball {
	if test ${tar_found} -ne 0; then
		echo -e "\nError in function extract_tarball: 'tar' command not found"
		exit 1
	fi

	local archive=$1
	local exclude=$2

	echo "Extracting tarball: ${archive}"
	if test -z "${exclude}"; then
		${cmd_tar} -xf "${archive}"
	else
		${cmd_tar} --exclude "${exclude}" -xf "${archive}"
	fi
	return $?
}

# only call this function from 'extract_archive'
function extract_zip {
	if test ${unzip_found} -ne 0; then
		echo -e "\nError in function extract_zip: 'unzip' command not found"
		exit 1
	fi

	local archive=$1

	echo "Extracting zip: ${archive}"
	${cmd_unzip} -qqo "${archive}"
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
	local name="$1"
	if test -z "${name}"; then
		echo -e "\nError in prepare function, name not set"
		exit 1
	fi

	local cfg="${dir_libs}/CONFIG/${name}"
	if test ! -f "${cfg}"; then
		local cfg="${dir_libs}/CONFIG-${name}"
	fi

	if test ! -f "${cfg}"; then
		echo -e "\nERROR: configuration not found:\n  ${dir_libs}/CONFIG/${name}: not found\n  ${cfg}: not found"
		exit 1
	fi

	# reset main values
	local ver dname fname extract_name src depends

	# only set this here for use in list_options.sh script
	local dir_config_root

	# reset options
	local patch_prune_level crlf_to_lf exclude_extract

	# reset commands
	local cmd_download cmd_extract

	# reset functions that can be defined in config files
	unset pre_dl post_dl pre_extract post_extract

	local rebuild=false

	# reset status
	local DOWNLOAD_DONE=false
	local EXTRACT_DONE=false
	local PREPARE_DONE=false

	# backup original name in case of rebuild
	local name_orig="${name}"

	# import configuration
	. "${cfg}"

	# detect rebuild
	if test "${name_orig}" != "${name}"; then
		echo -e "\nDoing re-build of ${name} ${ver}"
		local rebuild=true
	fi

	# check values imported from configuration
	if test -z "${ver}" || test -z "${dname}"; then
		echo -e "\nERROR: malformed configuration, ver & dname must be set: ${cfg}"
		exit 1
	fi

	# src & fname must be set if not using a custom download command
	if test -z "${cmd_download}"; then
		if test -z "${fname}" || test -z "${src}"; then
			echo -e "\nERROR: malformed configuration, fname & src must be set: ${cfg}"
			exit 1
		fi
	fi

	# FIXME: do this in build function
	# build dependencies first
	if ! ${prepare_only} && test ! -z "${depends}"; then
		echo -e "\nBuilding dependencies for ${name_orig} ${ver}"

		local dep_name
		for dep_name in ${depends[@]}; do
			prepare "${dep_name}"
			local ret=$?
			if test ${ret} -ne 0; then
				echo -e "\nCould not prepare ${name_orig} ${ver}"
				return ${ret}
			fi
		done
	fi

	echo -e "\nPreparing ${name_orig} ${ver}"
	echo "Using configuration: ${cfg}"

	local file_lib_prepare="${dir_build}/PREPARE-${name_orig}-${ver}"

	if test -f "${file_lib_prepare}"; then
		. "${file_lib_prepare}"
	fi

	if ${rebuild} && ! ${PREPARE_DONE}; then
		# source should already be prepared from original build
		PREPARE_DONE=true
		echo "PREPARE_DONE=true" >> "${file_lib_prepare}"
	fi

	if ${PREPARE_DONE}; then
		echo "Using previously prepared sources for ${name_orig} ${ver}"
		if ${prepare_only}; then
			return
		fi
	else
		cd "${dir_src}"
		package="${dir_src}/${fname}"

		if ${DOWNLOAD_DONE}; then
			echo "Not re-downloading sources for ${name_orig} ${ver}"
		else
			# clean up old files
			if test -d "${dir_src}/${dname}"; then
				echo "Removing old source directory: ${dir_src}/${dname}"
				rm -rf "${dir_src}/${dname}"
			fi

			# pre-download operations
			if test ! -z "$(type -t pre_dl)" && test "$(type -t pre_dl)" == "function"; then
				echo -e "\nRunning pre-download commands"
				pre_dl
			fi

			if test ! -z "${cmd_download}"; then
				"${cmd_download[@]}"
				dl_ret=$?
			else
				if test -f "${package}"; then
					echo "Found package: ${fname}"
					dl_ret=0
				else
					download_source "${src}" "${fname}"
					dl_ret=$?
				fi
			fi

			if test ${dl_ret} -ne 0; then
				echo -e "\nAn error occurred while downloading ${name_orig} ${ver}"
				exit ${dl_ret}
			fi

			# post-download operations
			if test ! -z "$(type -t post_dl)" && test "$(type -t post_dl)" == "function"; then
				echo -e "\nRunning post-download commands"
				post_dl
			fi

			echo "DOWNLOAD_DONE=true" >> "${file_lib_prepare}"
		fi

		if ${EXTRACT_DONE} && test -d "${dir_src}/${dname}"; then
			echo "Not re-extracting sources for ${name_orig} ${ver}"
		else
			# clean up old files
			if test -d "${dir_src}/${dname}"; then
				echo "Removing old source directory: ${dir_src}/${dname}"
				rm -rf "${dir_src}/${dname}"
			fi

			# pre-extract operations
			if test ! -z "$(type -t pre_extract)" && test "$(type -t pre_extract)" == "function"; then
				echo -e "\nRunning pre-extract commands"
				pre_extract
			fi

			if test ! -z "${cmd_extract}"; then
				"${cmd_extract[@]}"
			else
				extract_archive "${package}" "${exclude_extract}"
			fi

			if test $? -ne 0; then
				echo -e "\nAn error occurred while extracting file: ${package}"
				exit 1
			fi

			# use standard naming convention
			if test ! -z "${extract_name}"; then
				if test -d "${dname}"; then
					rm -rf "${dname}"
				fi
				mv "${extract_name}" "${dname}"
			fi

			# FIXME: how to do this in POST_EXTRACT?
			if test ! -z "${crlf_to_lf}"; then
				for F in ${crlf_to_lf[@]}; do
					sed -i -e 's|\r$||g' "${dir_src}/${dname}/${F}"
					if test $? -ne 0; then
						echo -e "\nAn error occurred while attempting to convert CRLF line endings to LF: ${dir_src}/${dname}/${F}"
						exit 1
					fi
				done
			fi

			cd "${dname}"

			# apply external/downloaded patches
			ext_patch_dir="${dir_libs}/patch/EXTERNAL-${name}-${ver}"
			if test -d "${ext_patch_dir}"; then
				if test -z "${patch_prune_level}"; then
					echo -e "\nERROR: patch_prune_level must be set when applying external patches"
					exit 1
				fi

				# FIXME: need to delete patches for older versions (probably before source download)
				for patch in $(find "${ext_patch_dir}" -maxdepth 1 -type f); do
					echo "Applying external patch: $(basename ${patch})"
					# XXX: downloaded patches may need a different "prune" level
					patch -p${patch_prune_level} -i "${patch}"
					ret=$?
					if test ${ret} -ne 0; then
						echo -e "\nAn error occurred while trying to apply external patch: $(basename ${patch})"
						exit ${ret}
					fi
				done
			fi

			# apply internal patches
			if test -d "${dir_libs}/patch/"; then
				patches=$(ls "${dir_libs}/patch/" | grep "^${name}-.*\.patch")
			fi

			for patch in ${patches}; do
				if ${os_win}; then
					local this_sys="win32"
				else
					local this_sys="${OSTYPE}"
				fi

				local p_name=$(echo ${patch} | sed -e "s|^${name}-||" -e 's|\.patch$||')
				local p_sys=$(echo ${p_name} | cut -d"-" -f2)
				local p_static_only=false
				if test "$(echo ${p_name} | cut -d"-" -f3)" == "static_only"; then
					p_static_only=true
				fi

				case "${p_sys}" in
					"any"|"all"|"${this_sys}")
						if test ${p_static_only} && ! ${static}; then
							# FIXME: better method for checking if static patch should be applied?
							echo "Ignoring static patch for non-static build: ${patch}"
						else
							echo "Applying patch: ${patch}"
							patch -p1 -i "${dir_libs}/patch/${patch}"
							ret=$?
							if test ${ret} -ne 0; then
								echo -e "\nAn error occurred while trying to apply patch: ${patch}"
								exit ${ret}
							fi
						fi
						;;
					*)
						echo "Ignoring patch for non-${this_sys} system: ${patch}"
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
				echo "EXTRACT_DONE=true" >> "${file_lib_prepare}"
			fi
		fi

		cd "${dir_libs}"
		echo "PREPARE_DONE=true" >> "${file_lib_prepare}"
		echo -e "\nFinished preparing ${name_orig} ${ver}"
	fi

	if ! ${prepare_only}; then
		build "${name_orig}"
		local ret=$?
		if test ${ret} -ne 0; then
			echo -e "\nCould not build ${name_orig} ${ver}"
			return ${ret}
		fi
	fi
)


build() (
	local name="$1"
	if test -z "${name}"; then
		echo -e "\nError in build function, name not set"
		exit 1
	fi

	local cfg="${dir_libs}/CONFIG/${name}"
	if test ! -f "${cfg}"; then
		local cfg="${dir_libs}/CONFIG-${name}"
	fi

	if test ! -f "${cfg}"; then
		echo -e "\nERROR: configuration not found:\n  ${dir_libs}/CONFIG/${name}: not found\n  ${cfg}: not found"
		exit 1
	fi

	# reset main values
	local ver dname

	# reset directories
	local dir_config_root

	# reset options (NOTE: exported env variables can NEVER be set to array)
	local cc cxx cflags cxxflags cppflags ldflags libs
	local config_opts libtype_opts

	# reset commands
	local cmd_config
	local cmd_build=(${CMD_MAKE})
	local cmd_install=(${CMD_MAKE} install)

	# reset functions that can be defined in config files
	unset pre_cfg post_cfg pre_build post_build pre_install post_install

	# reset status
	local CONFIG_DONE=false
	local BUILD_DONE=false
	local INSTALL_DONE=false

	# backup original name in case of rebuild
	local name_orig="${name}"

	# import configuration
	. "${cfg}"

	CC="${cc[@]}"
	CXX="${cxx[@]}"
	CFLAGS="${cflags[@]}"
	CXXFLAGS="${cxxflags[@]}"
	CPPFLAGS="${cppflags[@]} -I${install_prefix}/include"
	LDFLAGS="${ldflags[@]} -L${install_prefix}/lib"
	LIBS="${libs[@]}"
	export CC CXX CFLAGS CXXFLAGS CPPFLAGS LDFLAGS LIBS

	local lib_build="${name_orig}-${ver}-${build_type}"
	local file_lib_install="${dir_build}/INSTALL-${lib_build}"

	# prevents re-build/re-install
	if test -f "${file_lib_install}"; then
		. "${file_lib_install}"
	fi

	if ${INSTALL_DONE}; then
		echo "Using previous install of ${name_orig} ${ver}"
	else
		local dir_lib_build="${dir_build}/${lib_build}"
		if ${BUILD_DONE}; then
			echo "Not re-building ${name_orig} ${ver}"
		else
			if test ! -d "${dir_lib_build}"; then
				mkdir -p "${dir_lib_build}"
			fi

			cd "${dir_lib_build}"

			if ${CONFIG_DONE}; then
				echo "Not re-configuring ${name_orig} ${ver}"
			else
				echo -e "\nConfiguring ${name_orig} ${ver} ..."

				# remove old cache if CONFIG_DONE == false
				find ./ -type f -delete
				find ./ -mindepth 1 -type d -empty -delete

				if test -z "${dir_config_root}"; then
					dir_config_root="${dir_src}/${dname}"
				fi

				# pre-configuration operations
				if test ! -z "$(type -t pre_cfg)" && test "$(type -t pre_cfg)" == "function"; then
					echo -e "\nRunning pre-config commands"
					pre_cfg
				fi

				if test -z "${cmd_config}"; then
					# add common config options
					config_opts+=(--prefix="${install_prefix}")

					if test "${libtype_opts}" != "N/A"; then
						if test -z "${libtype_opts}"; then
							# use defaults
							if ${static}; then
								libtype_opts=(--enable-static=yes --enable-shared=no)
							else
								libtype_opts=(--enable-static=no --enable-shared=yes)
							fi
						fi
					else
						unset libtype_opts
					fi

					config_opts+=(${libtype_opts[@]})

					"${dir_config_root}/configure" ${config_opts[@]}
					if test $? -ne 0; then
						echo -e "\nAn error occurred while configuring ${name_orig} ${ver}"
						exit 1
					fi
				else
					# common values for CMake
					echo "${cmd_config[0]}" | grep -q "cmake$"
					if test $? -eq 0; then
						if test "${OSTYPE}" == "msys"; then
							cmd_config+=(-G "MSYS Makefiles")
						fi
						cmd_config+=(-DCMAKE_INSTALL_PREFIX=${install_prefix} -DCMAKE_PREFIX_PATH=${install_prefix})
						cmd_config+=(-DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_FIND_LIBRARY_SUFFIXES=".a")

						if test "${libtype_opts}" != "N/A"; then
							if test -z "${libtype_opts}"; then
								# use defaults
								if ${static}; then
									libtype_opts=(-DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF)
								else
									libtype_opts=(-DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON)
								fi
							fi
						else
							# make sure libtype_opts is empty if set to "N/A"
							unset libtype_opts
						fi

						# add build type to config options
						if test ! -z "${libtype_opts}"; then
							config_opts+=(${libtype_opts[@]})
						fi

						# add config options to config command & source directory as final argument in config command
						cmd_config+=(${config_opts[@]} "${dir_config_root}")
					fi

					# common values for meson
					echo "${cmd_config[0]}" | grep -q "meson$"
					if test $? -eq 0; then
						if test "${libtype_opts}" != "N/A"; then
							if test -z "${libtype_opts}"; then
								# use defaults
								if ${static}; then
									libtype_opts=(--default-library=static)
								else
									libtype_opts=(--default-library=shared)
								fi
							fi
						else
							# make sure to libtype_opts is empty if set to "N/A"
							unset libtype_opts
						fi

						# add default options
						config_opts=(--prefix=${install_prefix} --buildtype=plain ${libtype_opts[@]} ${config_opts[@]})
						cmd_config+=(${config_opts[@]} "${dir_config_root}")
					fi

					# this is usually used for source that does not use build generators like GNU Autotools or CMake
					if test "${cmd_config[0]}" == "copy"; then
						local ifs_orig=${IFS}
						IFS=$'\n'
						for FD in $(find "${dir_src}/${dname}" -mindepth 1 -maxdepth 1); do
							cp -r "${FD}" ./
						done
						IFS=${ifs_orig}
					else
						"${cmd_config[@]}"
						if test $? -ne 0; then
							echo -e "\nAn error occurred while configuring ${name_orig} ${ver}"
							exit 1
						fi
					fi
				fi

				# post-configuration operations
				if test ! -z "$(type -t post_cfg)" && test "$(type -t post_cfg)" == "function"; then
					echo -e "\nRunning post-config commands"
					post_cfg
				fi

				echo "CONFIG_DONE=true" >> "${file_lib_install}"
			fi

			echo -e "\nBuilding ${name_orig} ${ver} ..."
			# pre-build operations
			if test ! -z "$(type -t pre_build)" && test "$(type -t pre_build)" == "function"; then
				echo -e "\nRunning pre-build commands"
				pre_build
			fi

			"${cmd_build[@]}"
			if test $? -ne 0; then
				echo -e "\nAn error occurred while building ${name_orig} ${ver}"
				exit 1
			fi

			# post-build operations
			if test ! -z "$(type -t post_build)" && test "$(type -t post_build)" == "function"; then
				echo -e "\nRunning post-build commands"
				post_build
			fi

			echo "BUILD_DONE=true" >> "${file_lib_install}"

			cd "${dir_libs}"
		fi

		cd "${dir_lib_build}"
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

		echo -e "\nInstalling ${name_orig} ${ver}"
		"${cmd_install[@]}"
		if test $? -ne 0; then
			echo -e "\nAn error occurred while installing ${name_orig} ${ver}"
			exit 1
		fi

		if ${clean_libtool}; then
			local lt_files=($(find "${install_prefix}/lib/" -type f -name "*.la"))
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

		echo "INSTALL_DONE=true" >> "${file_lib_install}"
		echo -e "\nFinished processing ${name_orig} ${ver}"

		cd "${dir_libs}"
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

if test ! -z "${build_libs}"; then
	builtin_libs="${build_libs}"
else
	# base libs in build order
	builtin_libs=(libiconv-reb freetype-reb cairo-reb SDL2_mixer wxSVG)
fi

if test ! -z "${NO_BUILD_LIBS}"; then
	for L in ${NO_BUILD_LIBS}; do
		builtin_libs=$(echo "${builtin_libs}" | sed -e "s|${L}||g")
	done
fi

# verbose make
export VERBOSE=1

if test -z "${CMD_MAKE}"; then
	CMD_MAKE="make"
fi

for name in ${builtin_libs[@]}; do
	echo -e "\nProcessing ${name} ..."
	prepare "${name}"
	ret=$?
	if test ${ret} -ne 0; then
		echo -e "\nExiting with errors"
		exit ${ret}
	fi
done

# for any packages that install DLLs into lib dir
if ${os_win} && test -d "${install_prefix}/lib/"; then
	mkdir -p "${install_prefix}/bin/"
	for dll in $(find "${install_prefix}/lib/" -mindepth 1 -maxdepth 1 -type f -name "*.[dD][lL][lL]"); do
		mv -v "${dll}" "${install_prefix}/bin/"
	done
fi

# unnecessary directories
nouse_dirs=("share/doc" "share/man")
for dir in ${nouse_dirs[@]}; do
	if test -d "${install_prefix}/${dir}"; then
		echo -e "\nRemoving unused dir: ${dir}"
		rm -rf "${install_prefix}/${dir}"
	fi
done

# remove empty directories
find "${install_prefix}/" -mindepth 1 -type d -empty -delete
