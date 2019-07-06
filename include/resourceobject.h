#ifndef MYABCS_RESOURCE_OBJECT_H
#define MYABCS_RESOURCE_OBJECT_H

#include <SDL2/SDL_mixer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/sound.h>
#include <wx/string.h>
#include <wx/window.h>


// TODO: move these to a better file for global inclusion
extern wxImage imageFromSVG(wxString filename, unsigned int width, unsigned int height);
extern wxImage imageFromSVG(unsigned char* data, unsigned int data_size, unsigned int width, unsigned int height);
extern wxImage imageFromSVG(const unsigned char* data, unsigned int data_size, unsigned int width, unsigned int height);


class ResourceObject {
public:
	ResourceObject() {}
	ResourceObject(const ResourceObject& ro);
	ResourceObject(wxString label, wxImage img);
	ResourceObject(wxString label, wxString category);
	~ResourceObject();
	wxString getLabel() { return objectLabel; }
	wxBitmap getBitmap() { return wxBitmap(objectImage); }
	bool playSound(wxWindow* source);
	wxString getVocalString() { return sndVocalString; }
	wxString getEffectString() { return sndEffectString; }
private:
	wxString objectLabel;
	wxImage objectImage;
	//Mix_Chunk* sndVocal;
	//Mix_Chunk* sndEffect;
	// FIXME: not true caching
	wxString sndVocalString;
	wxString sndEffectString;

protected:
	void loadImage(wxString img);
	void loadSound(wxString snd);
};


#endif /* MYABCS_RESOURCE_OBJECT_H */
