#ifndef MYABCS_PATHS_H
#define MYABCS_PATHS_H

#include <string>


// FIXME: this should be inaccessible
static std::string executable;
static std::string dir_root;
static std::string dir_pic;
static std::string dir_snd;

static void initPaths(std::string path, std::string exe) {
	executable = exe;
	dir_root = path;
	dir_pic = dir_root + "/pic";
	dir_snd = dir_root + "/sound";
}

static std::string getRootDir() {
	return dir_root;
}


#endif /* MYABCS_PATHS_H */
