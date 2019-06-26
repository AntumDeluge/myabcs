#ifndef MYABCS_RESOURCE_OBJECT_H
#define MYABCS_RESOURCE_OBJECT_H

#include <SDL2/SDL_mixer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/sound.h>
#include <wx/string.h>


class ResourceObject {
public:
	ResourceObject(wxString label, wxImage img, wxString snd);
	ResourceObject(wxString label, wxString img, wxString snd);
	~ResourceObject();
	const wxString getLabel() { return objectLabel; }
	const wxBitmap getBitmap() { return wxBitmap(objectImage); }
	bool playSound();
private:
	const wxString objectLabel;
	const wxImage objectImage;
	const Mix_Chunk* objectSound;

protected:
	void loadSound(wxString snd);
};


#endif /* MYABCS_RESOURCE_OBJECT_H *?
