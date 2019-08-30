#!/usr/bin/env bash

script_name="$(basename $0)"
root_dir="$(dirname $0)/../"
cd "${root_dir}"
root_dir="$(pwd)"
libs_dir="${root_dir}/libraries"
cd "${libs_dir}"

if test ! -z $2; then
	lib_name=$1
	lib_ver=$2
else
	IFS='-' read -r -a IN <<< "$1"
	lib_name=${IN[0]}
	if test ${#IN[@]} -gt 1; then
		lib_ver=${IN[1]}
	fi
fi

if test -z "${lib_name}"; then
	echo -e "\nPlease supply library directory name (usually as <name>-<version>): ${script_name} <directory>"
	show_available_libs
	exit 1
fi

if test ! -z "${lib_ver}"; then
	full_name="${lib_name}-${lib_ver}"
else
	full_name="${lib_name}"
fi

avail_configs=$(find "${libs_dir}/" -mindepth 1 -maxdepth 1 -type f -name "CONFIG-*")
if test -d "${libs_dir}/CONFIG/"; then
	avail_configs+=$(find "${libs_dir}/CONFIG/" -mindepth 1 -maxdepth 1 -type f)
fi
avail_configs=$(echo "${avail_configs}" | sed -e "s|${libs_dir}/CONFIG/||g" -e "s|${libs_dir}/||g" -e 's|^CONFIG-||g')

function show_available_configs {
	if test ${#avail_configs[@]} -eq 0; then
		echo -e "\nNo available configurations to list"
	else
		echo -e "\nAvailable configurations:\n\n"
		for cfg in ${avail_configs}; do
			echo "  ${cfg}"
		done
	fi
}

found_config=false
config_path=
function is_config_available {
	local libname=$1
	if test -z "${libname}"; then
		echo -e "\nERROR: lib name not specified in is_config_available function"
		return 1
	fi

	for cfg in ${avail_configs}; do
		if test "${libname}" == "${cfg}"; then
			# get path to configuration file
			if test -f "${libs_dir}/CONFIG-${libname}"; then
				config_path="${libs_dir}/CONFIG-${libname}"
			else
				config_path="${libs_dir}/CONFIG/${libname}"
			fi

			return 0
		fi
	done

	return 1
}

avail_srcs=$(find "${libs_dir}/source/" -maxdepth 1 -type d -exec echo  {} \; | sed -e "s|${libs_dir}/source/||")
function show_available_libs {
	if test ${#avail_srcs[@]} -eq 0; then
		echo -e "\nNo available sources to list"
	else
		echo -e "\nAvailable libraries:\n\n"
		for AS in ${avail_srcs[@]}; do
			echo "  ${AS}"
		done
	fi
}

possible_matches=()
function show_possible_matches {
	if test ${#possible_matches[@]} -eq 0; then
		echo -e "\nDid not find any possible matches"
	else
		echo -e "\nPossible matches:\n\n"
		for PM in ${possible_matches[@]}; do
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

	local name="${lib_name}"
	local DIR_SRC="${libs_dir}/source"
	dir_config_root="${target_dir}"

	if ${found_config}; then
		# check for custom config root
		. "${config_path}"
	fi

	cd "${target_dir}"

	local cfg_gnu=false
	local cfg_cmake=false
	local cfg_meson=false

	if test -f "${dir_config_root}/configure"; then
		cfg_gnu=true
	fi
	if test -f "${dir_config_root}/CMakeLists.txt"; then
		cfg_cmake=true
	fi
	if test -f "${dir_config_root}/meson.build"; then
		cfg_meson=true
	fi

	if ${cfg_gnu}; then
		echo -e "\n${name}: Found GNU Autotools configure script"
		"${dir_config_root}/configure" --help
	fi
	if ${cfg_cmake}; then
		echo -e "\n${name}: Found CMake configuration"
		tmp_dir="tmp-0000000001"
		mkdir "${tmp_dir}"
		cd "${tmp_dir}"
		cmake ${config_opts[@]} ${libtype_opts[@]} "${dir_config_root}" > /dev/null 2>&1
		cmake -LA  ${config_opts[@]} ${libtype_opts[@]} "${dir_config_root}" | awk '{if(f)print} /-- Cache values/{f=1}'
		cd ../
		rm -r "${tmp_dir}"
	fi
	if ${cfg_meson}; then
		if test ! -f "${dir_config_root}/meson_options.txt"; then
			echo -e "\n${name}: ERROR: Found Meson Build configuration, but \"meson_options.txt\" is missing"
			exit 1
		fi
		echo -e "\n${name}: Found Meson Build configuration"
		cat "${dir_config_root}/meson_options.txt"
	fi

	if test ! ${cfg_gnu} && test ! ${cfg_cmake} && test ! ${cfg_meson}; then
		echo -e "\nERROR: No recognizable configuration found\nTry deleting the directory \"${target_dir}\" & run again"
		exit 1
	fi

	cd "${libs_dir}"
}

is_config_available "${lib_name}"
if test $? -eq 0; then
	found_config=true
fi

# FIXME: matching should be case insensitive
exact_match=
for dir in ${avail_srcs}; do
	if test "${dir}" == "${full_name}"; then
		exact_match="${dir}"
		break
	fi
	IFS='-' read -r -a src <<< "${dir}"
	# FIXME: use regular exparession
	if test "${src[0]}" == "${lib_name}"; then
		possible_matches+=("${dir}")
	fi
done

if test -z "${exact_match}" && test ${#possible_matches[@]} -eq 0; then
	if ! ${found_config}; then
		target_dir="${libs_dir}/source/${full_name}"
		echo -e "\nDid not find library directory: ${target_dir}"
		show_available_libs
		exit 1
	else
		echo -e "\nFound config for ${lib_name}"
		cd "${root_dir}"
		make build-libs BUILD_LIBS="extract ${lib_name}"
		cd "${libs_dir}"
		echo -e "\nNow re-run \`./${script_name} ${lib_name}\`"
		exit 0
	fi
elif test -z "${exact_match}" && test ${#possible_matches[@]} -eq 1; then
	exact_match="${possible_matches[0]}"
	echo -e "\nFound single possible match: ${exact_match}"
fi

if test ! -z "${exact_match}"; then
	target_dir="${libs_dir}/source/${exact_match}"
	show_lib_options "${target_dir}"
else
	show_possible_matches
fi
