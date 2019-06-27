#ifndef MYABCS_SOUND_H
#define MYABCS_SOUND_H

#include <SDL2/SDL_mixer.h>
#include <string>
#include <wx/sound.h>


class SoundFile {
public:
	SoundFile(const std::string filename) { file = filename; }
	std::string toString() { return file; }
private:
	std::string file;
};


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
private:
	//SoundFile* soundFile;
};

extern SoundPlayer* soundPlayer;

extern void initializeSounds();

extern bool soundIsInitialized();

#endif /* MYABCS_SOUND_H */
