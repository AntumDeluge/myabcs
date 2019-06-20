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


class LegacySound {
public:
	LegacySound(const wxString filename);
	bool Play(unsigned flags=wxSOUND_ASYNC);
	bool Play(const wxString filename, unsigned flags=wxSOUND_ASYNC);
	//static bool IsPlaying();
	void Stop();
private:
	wxString sound;
};


class SoundPlayer {
public:
	SoundPlayer();
	~SoundPlayer();
	void init();
	void load(const std::string filename);
	void load(const wxString filename);
	void unload();
	void play();
	void stop();
	bool isReady();
	bool isPlaying();
private:
	//SoundFile* soundFile;
};

static SoundPlayer* soundPlayer;

extern void initializeSounds();

#endif /* MYABCS_SOUND_H */
