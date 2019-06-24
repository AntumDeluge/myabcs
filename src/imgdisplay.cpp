#include <wx/filefn.h>
#include <wx/mstream.h>
#include <wxSVG/SVGDocument.h>

#include "imgdisplay.h"
#include "log.h"
#include "res/failsafe.png.h"


ImageDisplay::ImageDisplay(wxWindow* parent, wxWindowID id, wxBitmap& label) :
wxStaticBitmap(parent, id, label) {
}

void ImageDisplay::SetImageG(wxString filename) {
	wxImage image;
	wxString png_filename, svg_filename;

	png_filename = filename;
	if (!png_filename.EndsWith(_T(".png"))) {
		png_filename = png_filename.Append(".png");
	}

	svg_filename = png_filename;
	if (!svg_filename.EndsWith(_T(".svg"))) {
		svg_filename = svg_filename.SubString(0, svg_filename.Len()-4).Append(_T("svg"));
	}

	// use PNG image by default
	if (wxFileExists(png_filename)) {
		// DEBUG:
		logMessage(wxString("WARNING: Using PNG image: ").Append(png_filename));

		image = wxImage(png_filename);
	} else if (wxFileExists(svg_filename)) {
		// load SVG data
		wxSVGDocument* svg = new wxSVGDocument();
		bool loaded = svg->Load(svg_filename);

		// FIXME: can't check if image displayed properly
		//logMessage(wxString("SVG loaded: ").Append(std::to_string(svg->IsOk())));
		if (!loaded) {
			logMessage(wxString("Loading SVG document failed: ").Append(svg_filename));
		}

		image = svg->Render(290, 290, NULL, true, true, NULL);
	} else {
		logMessage(wxString("ERROR: Could not load image: ").Append(filename));

		// load embedded failsafe image data
		wxMemoryInputStream is(&failsafe_png_v[0], failsafe_png_v.size()); // convert PNG data into input stream
		image = wxImage(is, wxBITMAP_TYPE_PNG);
	}

	wxStaticBitmap::SetBitmap(wxBitmap(image));
}
