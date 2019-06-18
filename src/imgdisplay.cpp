#include <wx/filefn.h>
#include <wx/log.h>
#include <wxSVG/SVGDocument.h>

#include "imgdisplay.h"


ImageDisplay::ImageDisplay(wxWindow* parent, wxWindowID id, wxBitmap& label) :
wxStaticBitmap(parent, id, label) {
}

void ImageDisplay::SetImageG(wxString filename) {
	wxImage image;
	wxString svg_filename = filename;
	if (!svg_filename.EndsWith(_T(".svg"))) {
		svg_filename = svg_filename.SubString(0, svg_filename.Len()-4).Append(_T("svg"));
	}

	// use SVG image if found
	if (wxFileExists(svg_filename)) {
		// load SVG data
		wxSVGDocument* svg = new wxSVGDocument();
		svg->Load(svg_filename);

		// FIXME: can't check if image displayed properly
		//wxLogGeneric(wxLOG_Info, wxString("SVG loaded: ").Append(std::to_string(svg->IsOk())));

		image = svg->Render(290, 290, NULL, true, true, NULL);
	} else {
		if (!wxFileExists(filename)) {
			wxLogGeneric(wxLOG_Info, wxString("ERROR: Could not load image: ").Append(filename));
			return;
		} else {
			image = wxImage(filename);
		}
	}

	wxStaticBitmap::SetBitmap(wxBitmap(image));
}
