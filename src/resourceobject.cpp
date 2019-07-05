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
	// load image data from filename string
	bool loaded = svgdoc->Load(filename);

	if (!loaded) {
		logMessage(wxString::Format("Loading SVG from file failed: %s", filename));
	}

	// XXX: need to clear svgdoc

	return svgdoc->Render(width, height, NULL, true, true, NULL);
}

wxImage imageFromSVG(unsigned char* data, unsigned int data_size, unsigned int width, unsigned int height) {
	// load image data from memory
	wxMemoryInputStream is(data, data_size); // convert SVG data into input stream
	bool loaded = svgdoc->Load(is);

	if (!loaded) {
		logMessage("Loading SVG data failed");
	}

	// XXX: need to clear svgdoc

	return svgdoc->Render(width, height, NULL, true, true, NULL);
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
	wxString filename = getImageFile(img);

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
	sndVocalString = getSoundFile(snd);
	// "effect" sounds are in sub-directory
	sndEffectString = getSoundFile(wxString::Format("effect/%s", snd));

	logMessage(wxString::Format("Loaded sound file: %s", sndVocalString));
}
