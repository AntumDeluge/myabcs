#include "paths.h"

#include <array>
#include <wx/filefn.h> // wxFileExists
#include <wx/list.h>

// DEBUG:
#include <iostream>

using namespace std;


static wxString executable;
static wxString dir_root;
static wxString dir_pic;

/** Supported image formats (in order of preference) */
static const array<wxString, 2> img_extensions { "png", "svg", };


void initPaths(wxString path, wxString exe) {
	executable = exe;
	dir_root = path;
	dir_pic = wxString::Format("%s/pic", dir_root);
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
