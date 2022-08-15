
/* Copyright © 2010-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "log.h"
#include "paths.h"
#include "resourceobject.h"
#include "sound.h"
#include "res/failsafe_img.h"

#include <wx/filefn.h>
#include <wx/mstream.h>
#include <wxSVG/SVGDocument.h>


// SVG document used for converting to wxImage
static wxSVGDocument* svgdoc = new wxSVGDocument();


wxImage imageFromSVG(wxString filename, unsigned int width, unsigned int height) {
	// DEBUG:
	logMessage(wxString::Format("Loading SVG image from file: %s", filename));

	wxImage img = wxImage();

	if (!wxFileExists(filename)) {
		logMessage(wxString::Format("Cannot load SVG image, file does not exist: %s", filename));
		return img;
	}

	// load image data from filename string
	bool loaded = svgdoc->Load(filename);

	if (!loaded) {
		logMessage(wxString::Format("Loading SVG file failed: %s", filename));
		return img;
	}

	img = svgdoc->Render(width, height, NULL, true, true, NULL);
	if (!img.IsOk()) {
		logMessage(wxString::Format("Image appears to be invalid: %s", filename));
	}

	// XXX: need to clear svgdoc
	return img;
}

wxImage imageFromSVG(unsigned char* data, unsigned int data_size, unsigned int width, unsigned int height) {
	// DEBUG:
	logMessage("Loading SVG image from memory");

	wxImage img = wxImage();

	// load image data from memory
	wxMemoryInputStream is(data, data_size); // convert SVG data into input stream
	bool loaded = svgdoc->Load(is);

	if (!loaded) {
		logMessage("Loading SVG data failed");
		return img;
	}

	img = svgdoc->Render(width, height, NULL, true, true, NULL);
	if (!img.IsOk()) {
		logMessage("Image appears to be invalid");
	}

	// XXX: need to clear svgdoc
	return img;
}

wxImage imageFromSVG(const unsigned char* data, unsigned int data_size, unsigned int width, unsigned int height) {
	return imageFromSVG((unsigned char*) data, data_size, width, height);
}


/** copy constructor */
ResourceObject::ResourceObject(const ResourceObject& ro) {
	objectLabel = ro.objectLabel;
	objectImage = ro.objectImage;
	sndVocalString = ro.sndVocalString;
	sndEffectString = ro.sndEffectString;
}

/** sound is loaded from external file */
ResourceObject::ResourceObject(wxString label, wxImage img)
		: objectLabel(label), objectImage(img) {
	// replace whitespace in filename with underscore
	label.Replace(_T(" "), _T("_"));

	loadSound(label);
}

ResourceObject::ResourceObject(wxString label, wxString category)
		: objectLabel(label) {
	// replace whitespace in filename with underscore
	label.Replace(_T(" "), _T("_"));

	// used for object when "main" game category is completed
	if (category == wxEmptyString) {
		loadImage(label);
		loadSound(_T("cheering"));
	} else {
		loadImage(wxString::Format("%s/%s", category, label));
		loadSound(label);
	}
}

ResourceObject::~ResourceObject() {
	/*
	if (sndVocal != NULL) Mix_FreeChunk(sndVocal);
	if (sndEffect != NULL) Mix_FreeChunk(sndEffect);
	*/
}

bool ResourceObject::playSound(wxWindow* source) {
	soundPlayer->play(source, sndVocalString, sndEffectString);
	return true;
}

void ResourceObject::loadImage(wxString img) {
	wxString filename = getImagePath(img);

	if (!filename) {
		logMessage(wxString::Format("ERROR: Could not find compatible image for: %s", img));

		// load embedded failsafe image data
		objectImage = imageFromSVG((unsigned char*) failsafe_svg, sizeof(failsafe_svg), 290, 290);
	} else if (filename.EndsWith(".svg") || filename.EndsWith(".SVG")) {
		objectImage = imageFromSVG(filename, 290, 290);
	} else {
		logMessage(wxString::Format("WARNING: Using bitmap image: %s", filename));

		objectImage = wxImage(filename);
	}
}

void ResourceObject::loadSound(wxString snd) {
	if (snd.IsEmpty()) {
		logError(_T("Cannot load sound from empty filename"));
		return;
	}

	// "vocal" sounds are located in root of sound directory
	sndVocalString = getSoundPath(snd);
	// "effect" sounds are in sub-directory
	sndEffectString = getSoundPath(wxString::Format("effect/%s", snd));

	logMessage(wxString::Format("Loaded sound file: %s", sndVocalString));
}
