#include <wxSVG/SVGDocument.h>

#include "imgdisplay.h"


ImageDisplay::ImageDisplay(wxWindow* parent, wxWindowID id, wxBitmap& label) :
wxStaticBitmap(parent, id, label) {
}

void ImageDisplay::SetImageG(wxString filename) {
	wxImage image;

	if (filename.EndsWith(_T(".svg"))) {
		// load SVG data
		wxSVGDocument* svg = new wxSVGDocument();
		svg->Load(filename);
		image = svg->Render(290, 290, NULL, true, true, NULL);
	} else {
		image = wxImage(filename);
	}

	wxStaticBitmap::SetBitmap(wxBitmap(image));
}
