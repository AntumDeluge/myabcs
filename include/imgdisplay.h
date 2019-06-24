#ifndef MYABCS_IMGDISPLAY_H
#define MYABCS_IMGDISPLAY_H

#include <wx/statbmp.h>

class ImageDisplay : public wxStaticBitmap {
public:
	ImageDisplay(wxWindow* parent, wxWindowID id, wxBitmap& label);
	void SetImageG(wxString filename);
};

#endif /* MYABCS_IMGDISPLAY_H */
