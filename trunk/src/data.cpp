
#include "data.h"

using namespace std;

const wstring prefix = ".";
const wstring dir_data = prefix + "/data";
const wstring dir_audio = dir_data + "/audio";
const wstring dir_image = dir_data + "/image";
const wstring dir_logo = dir_data + "/logo";

const wstring getPrefix() {
    return prefix;
}

const wstring getDirData() {
    return dir_data;
}

const wstring getDirAudio() {
    return dir_audio;
}

const wstring getDirImage() {
    return dir_image();
}

const wstring getDirLogo() {
    return dir_logo;
}
