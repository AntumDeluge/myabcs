#ifndef MYABCS_RESOURCE_OBJECT_H
#define MYABCS_RESOURCE_OBJECT_H

#include <SDL2/SDL_mixer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/sound.h>
#include <wx/string.h>


class ResourceObject {
public:
	//ResourceObject() : objectSound(NULL) {};
	//ResourceObject(ResourceObject& ro);
	ResourceObject(wxString label, wxImage img);
	ResourceObject(wxString label, wxString category);
	~ResourceObject();
	wxString getLabel() { return objectLabel; }
	wxBitmap getBitmap() { return wxBitmap(objectImage); }
	bool playSound();
private:
	wxString objectLabel;
	wxImage objectImage;
	Mix_Chunk* sndVocal;
	Mix_Chunk* sndEffect;

protected:
	void loadImage(wxString img);
	void loadSound(wxString snd);
};


#endif /* MYABCS_RESOURCE_OBJECT_H */
