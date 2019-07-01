#ifndef MYABCS_SOUND_H
#define MYABCS_SOUND_H

#include <SDL2/SDL_mixer.h>
#include <wx/string.h>


class SoundPlayer {
public:
	SoundPlayer() {};
	~SoundPlayer();
	void init();
	void load(wxString primary, wxString auxiliary);
	void load(wxString primary) { return load(primary, wxEmptyString); }
	void play();
	void play(wxString primary, wxString auxiliary);
	void play(wxString primary) { return play(primary, wxEmptyString); }
	void stop();
	bool isLoaded(const wxString filename);
	bool isReady();
	bool isPlaying();
};

extern SoundPlayer* soundPlayer;

extern void initSound();

extern bool soundIsInitialized();

#endif /* MYABCS_SOUND_H */
