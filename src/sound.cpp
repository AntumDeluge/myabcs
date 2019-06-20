#include <SDL2/SDL.h>
#include <wx/filefn.h>

#include "log.h"
#include "sound.h"


static Mix_Chunk* chunk = NULL;
static int channel;


SoundPlayer::SoundPlayer() {}

SoundPlayer::~SoundPlayer() {
	unload();
	Mix_CloseAudio();
	SDL_Quit();
}

void SoundPlayer::init() {
	// initialize SDL
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		logError(wxString("Unable to initialize SDL: ").Append(SDL_GetError()));
		return;
	}

	logMessage(_T("Initialized SDL audio"));

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 1, 4096) != 0) {
		logError(wxString("Unable to open audio mixer: ").Append(Mix_GetError()));
		return;
	}

	logMessage(_T("Opened audio mixer"));
}

void SoundPlayer::load(const std::string filename) {
	// ensure chunk is NULL every time this method is called
	unload();

	if (!wxFileExists(filename)) {
		logError(wxString("Cannot load sound, file not found: ").Append(filename));
		return;
	}

	chunk = Mix_LoadWAV(filename.c_str());

	if (chunk == NULL) {
		wxString errmsg = wxString("Unable to load sound file \"").Append(filename).Append("\": ").Append(Mix_GetError());
		logError(errmsg);
		return;
	}

	logMessage(wxString("Loaded sound file: ").Append(filename));
}

void SoundPlayer::load(const wxString filename) {
	return load(std::string(filename.mb_str()));
}

void SoundPlayer::unload() {
	Mix_FreeChunk(chunk);
	chunk = NULL;
}

void SoundPlayer::play() {
	if (!isReady()) {
		logError(_T("Audio not loaded, cannot play sound"));
		return;
	}

	channel = Mix_PlayChannel(-1, chunk, 0);
	if (channel != 0) {
		logError(wxString("Playing sound failed: ").Append(Mix_GetError()));
		return;
	}

	logMessage(wxString("Playing sound ..."));

	// wait for sound to stop playing
	while (isPlaying());
}

void SoundPlayer::stop() {
	if (!isPlaying()) {
		logMessage(wxLOG_Warning, _T("Sound is already stopped"));
		return;
	}

	Mix_HaltChannel(channel);

	logMessage(_T("Stopped playing sound"));
}

bool SoundPlayer::isReady() {
	return chunk != NULL;
}

bool SoundPlayer::isPlaying() {
	return Mix_Playing(channel) != 0;
}


void initializeSounds() {
	soundPlayer = new SoundPlayer();
	soundPlayer->init();
}
