#include "log.h"
#include "resourceobject.h"

#include <wx/filefn.h>


/** sound is loaded from external file */
ResourceObject::ResourceObject(wxString label, wxImage img, wxString snd)
		: objectLabel(label), objectImage(img), objectSound(NULL) {
	loadSound(snd);
}

ResourceObject::ResourceObject(wxString label, wxString img, wxString snd)
		: objectLabel(label), objectImage(img), objectSound(NULL) {
	loadSound(snd);
}

ResourceObject::~ResourceObject() {
	Mix_FreeChunk(objectSound);
	delete objectSound;
}

bool ResourceObject::PlaySound() {
	return false;
}

void ResourceObject::loadSound(wxString snd) {
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

	logMessage(wxLOG_Debug, wxString("Loaded sound file: ").Append(snd));
}
