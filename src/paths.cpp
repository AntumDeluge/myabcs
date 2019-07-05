#include "paths.h"

#include <array>
#include <wx/filefn.h> // wxFileExists
#include <wx/list.h>

// DEBUG:
#include <iostream>

using namespace std;


static wxString executable;
static wxString dir_root;
static wxString dir_data;
static wxString dir_pic;
static wxString dir_snd;

/** Supported image formats (in order of preference) */
static const array<wxString, 2> img_extensions { "png", "svg", };
/** Support sound formats (in order of preference) */
static const array<wxString, 4> snd_extensions { "oga", "ogg", "flac", "wav", };


void initPaths(wxString path, wxString exe) {
	executable = exe;
	dir_root = path;
	dir_data = wxString::Format("%s/data", dir_root);
	dir_pic = wxString::Format("%s/image", dir_data);
	dir_snd = wxString::Format("%s/audio", dir_data);
}

wxString getExecutable() { return executable; }
wxString getRootDir() { return dir_root; }


/** Retrieves an image for display.
 *
 * @param img A path relative to `dir_pic` without extensions (e.g. "main/airplane")
 * @return Absolute path with extension or `wxEmptyString` if no suitable image is found
 */
wxString getImageFile(wxString img) {
	// concatenate image directory
	img = wxString::Format("%s/%s", dir_pic, img);

	for (wxString ext : img_extensions) {
		wxString absolute = wxString::Format("%s.%s", img, ext);
		if (wxFileExists(absolute)) {
			return absolute;
		}

		// check for uppercase extension
		absolute = wxString::Format("%s.%s", img, ext.Upper());
		if (wxFileExists(absolute)) {
			return absolute;
		}
	}

	return wxEmptyString;
}

/** Retrieves a sound file for playing.
 *
 * @param snd A path relative to `dir_snd` without extension (e.g. "effect/xun")
 * @return Absolute path with extension or `wxEmptyString` if no suitable sound is found
 */
wxString getSoundFile(wxString snd) {
	// concatenate sound directory
	snd = wxString::Format("%s/%s", dir_snd, snd);

	for (wxString ext : snd_extensions) {
		wxString absolute = wxString::Format("%s.%s", snd, ext);
		if (wxFileExists(absolute)) {
			return absolute;
		}

		// check for uppercase extension
		absolute = wxString::Format("%s.%s", snd, ext.Upper());
		if (wxFileExists(absolute)) {
			return absolute;
		}
	}

	return wxEmptyString;
}
