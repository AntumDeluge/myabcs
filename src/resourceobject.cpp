#include "log.h"
#include "resourceobject.h"
#include "res/failsafe.png.h"

#include <wx/filefn.h>
#include <wx/mstream.h>
#include <wxSVG/SVGDocument.h>


// directories where image & sound files are stored
static const wxString dir_images("pic/");
static const wxString dir_vocals("sound/");
static const wxString dir_effects("sound/effect/");

// channel for playing audio
static int channel;


/** sound is loaded from external file */
ResourceObject::ResourceObject(wxString label, wxImage img)
		: objectLabel(label), objectImage(img) {
	loadSound(label);
}

ResourceObject::ResourceObject(wxString label, wxString category)
		: objectLabel(label) {
	loadImage(category.Append("/").Append(label));
	loadSound(label);
}

ResourceObject::~ResourceObject() {
	if (sndVocal != NULL) Mix_FreeChunk(sndVocal);
	if (sndEffect != NULL) Mix_FreeChunk(sndEffect);
}

bool ResourceObject::playSound() {
	if (sndVocal == NULL) {
		logError(_T("Audio not loaded, cannot play sound"));
		return false;
	}

	channel = Mix_PlayChannel(-1, sndVocal, 0);
	if (channel != 0) {
		logError(wxString("Playing sound failed: ").Append(Mix_GetError()));
		return false;
	}

	// wait for sound to stop playing
	while (Mix_Playing(channel) != 0);

	// optional sound effect
	if (sndEffect != NULL) {
		channel = Mix_PlayChannel(-1, sndEffect, 0);
		if (channel != 0) {
			logError(wxString("Playing sound effect failed: ").Append(Mix_GetError()));
			// don't return here because sound effect is not essential
		}

		// wait for sound effect to stop playing
		while (Mix_Playing(channel) != 0);
	}

	return true;
}

void ResourceObject::loadImage(wxString img) {
	if (!img.StartsWith(dir_images)) {
		img = wxString(dir_images).Append(img);
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
	if (snd.IsEmpty()) {
		logError(_T("Cannot load sound from empty filename"));
		return;
	}

	if (!snd.EndsWith(_T(".wav"))) {
		snd = snd.Append(_T(".wav"));
	}

	wxString vocal = wxString(dir_vocals).Append(snd);
	wxString effect = wxString(dir_effects).Append(snd);

	const wxString ogg_vocal = vocal.Left(vocal.Len() - 3).Append("oga");
	const wxString ogg_effect = effect.Left(effect.Len() -3).Append("oga");

	// Vorbis audio takes priority
	if (wxFileExists(ogg_vocal)) {
		vocal = ogg_vocal;
	}
	if (wxFileExists(ogg_effect)) {
		effect = ogg_effect;
	}

	if (!wxFileExists(vocal)) {
		logError(wxString("Cannot load sound, file not found: ").Append(vocal));
		return;
	}

	sndVocal = Mix_LoadWAV(vocal.c_str());

	if (sndVocal == NULL) {
		wxString errmsg = wxString("Unable to load sound file \"").Append(vocal).Append("\": ").Append(Mix_GetError());
		logError(errmsg);
		return;
	}

	// optional sound effect
	if (wxFileExists(effect)) {
		sndEffect = Mix_LoadWAV(effect.c_str());
	}

	logMessage(wxString("Loaded sound file: ").Append(vocal));
}
