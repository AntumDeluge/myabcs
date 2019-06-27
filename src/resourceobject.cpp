#include "log.h"
#include "resourceobject.h"
#include "res/failsafe.png.h"

#include <wx/filefn.h>
#include <wx/mstream.h>
#include <wxSVG/SVGDocument.h>


// channel for playing audio
static int channel;


/** copy constructor */
/*
ResourceObject::ResourceObject(ResourceObject& ro)
		: objectLabel(ro.objectLabel), objectImage(ro.objectImage), objectSound(ro.objectSound) {}
*/

/** sound is loaded from external file */
ResourceObject::ResourceObject(wxString label, wxImage img, wxString snd)
		: objectLabel(label), objectImage(img), objectSound(NULL) {
	loadSound(snd);
}

ResourceObject::ResourceObject(wxString label, wxString img, wxString snd)
		: objectLabel(label), objectSound(NULL) {
	loadImage(img);
	loadSound(snd);
}

ResourceObject::~ResourceObject() {
	Mix_FreeChunk(objectSound);
	delete objectSound;
}

bool ResourceObject::playSound() {
	if (objectSound == NULL) {
		logError(_T("Audio not loaded, cannot play sound"));
		return false;
	}

	channel = Mix_PlayChannel(-1, objectSound, 0);
	if (channel != 0) {
		logError(wxString("Playing sound failed: ").Append(Mix_GetError()));
		return false;
	}

	// wait for sound to stop playing
	while (Mix_Playing(channel) != 0);

	return true;
}

void ResourceObject::loadImage(wxString img) {
	if (!img.StartsWith(_T("pic/"))) {
		img = wxString("pic/").Append(img);
	}

	wxString png_filename, svg_filename;

	png_filename = img;
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

		objectImage = wxImage(png_filename, wxBITMAP_TYPE_PNG);
	} else if (wxFileExists(svg_filename)) {
		// load SVG data
		wxSVGDocument* svg = new wxSVGDocument();
		bool loaded = svg->Load(svg_filename);

		// FIXME: can't check if image displayed properly
		//logMessage(wxString("SVG loaded: ").Append(std::to_string(svg->IsOk())));
		if (!loaded) {
			logMessage(wxString("Loading SVG document failed: ").Append(svg_filename));
		}

		objectImage = wxImage(svg->Render(290, 290, NULL, true, true, NULL));
	} else {
		logMessage(wxString("ERROR: Could not load image: ").Append(img));

		// load embedded failsafe image data
		wxMemoryInputStream is(&failsafe_png_v[0], failsafe_png_v.size()); // convert PNG data into input stream
		objectImage = wxImage(is, wxBITMAP_TYPE_PNG);
	}
}

void ResourceObject::loadSound(wxString snd) {
	if (!snd.EndsWith(_T(".wav"))) {
		snd = snd.Append(_T(".wav"));
	}

	if (!snd.StartsWith(_T("sound/"))) {
		snd = wxString(_T("sound/")).Append(snd);
	}

	// Vorbis audio takes priority
	const wxString ogg_snd = snd.Left(snd.Len() - 3).Append("oga");
	if (wxFileExists(ogg_snd)) {
		snd = ogg_snd;
	}

	if (snd.IsEmpty()) {
		logError(_T("Cannot load sound from empty filename"));
		return;
	}

	if (!wxFileExists(snd)) {
		logError(wxString("Cannot load sound, file not found: ").Append(snd));
		return;
	}

	objectSound = Mix_LoadWAV(snd.c_str());

	if (objectSound == NULL) {
		wxString errmsg = wxString("Unable to load sound file \"").Append(snd).Append("\": ").Append(Mix_GetError());
		logError(errmsg);
		return;
	}

	logMessage(wxString("Loaded sound file: ").Append(snd));
}
