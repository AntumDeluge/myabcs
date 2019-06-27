#ifndef MYABCS_SOUND_H
#define MYABCS_SOUND_H

#include <SDL2/SDL_mixer.h>
#include <string>
#include <wx/sound.h>


class SoundPlayer {
public:
	SoundPlayer();
	~SoundPlayer();
	void init();
	void load(const wxString filename);
	void load(const std::string filename);
	void unload();
	void play();
	void play(wxString filename);
	void play(const std::string filename);
	void stop();
	bool isLoaded(const wxString filename);
	bool isReady();
	bool isPlaying();
};

extern SoundPlayer* soundPlayer;

extern void initializeSounds();

extern bool soundIsInitialized();

#endif /* MYABCS_SOUND_H */
