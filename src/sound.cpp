#include "log.h"
#include "sound.h"

#include <SDL2/SDL.h>
#include <wx/filefn.h>


SoundPlayer* soundPlayer;

// check for sound intializations
static bool initialized = false;

static Mix_Chunk* primaryChunk = NULL;
static int channel;
static wxString primarySound;


void unloadChunks() {
	primaryChunk = NULL;
	primarySound.Empty();
}


SoundPlayer::~SoundPlayer() {
	Mix_FreeChunk(primaryChunk);
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

void SoundPlayer::load(wxString primary) {
	// ensure sound chunks are NULL every time this method is called
	unloadChunks();

	// Vorbis audio takes priority
	const wxString ogg_filename = primary.Left(primary.Len() - 3).Append("oga");
	if (wxFileExists(ogg_filename)) {
		primary = ogg_filename;
	}

	if (isLoaded(primary)) {
		logMessage(wxString("Not re-loading sound: ").Append(primary));
		return;
	}

	if (primary.IsEmpty()) {
		logError(_T("Cannot load sound from empty filename"));
		return;
	}

	if (!wxFileExists(primary)) {
		logError(wxString("Cannot load sound, file not found: ").Append(primary));
		return;
	}

	primaryChunk = Mix_LoadWAV(primary.c_str());

	if (primaryChunk == NULL) {
		wxString errmsg = wxString("Unable to load sound file \"").Append(primary).Append("\": ").Append(Mix_GetError());
		logError(errmsg);
		return;
	}

	primarySound = primary;

	logMessage(wxString("Loaded sound file: ").Append(primary));
}

void SoundPlayer::play() {
	if (!isReady()) {
		logError(_T("Audio not loaded, cannot play sound"));
		return;
	}

	channel = Mix_PlayChannel(-1, primaryChunk, 0);
	if (channel != 0) {
		logError(wxString("Playing sound failed: ").Append(Mix_GetError()));
		return;
	}

	logMessage(wxString("Playing sound ..."));

	// wait for sound to stop playing
	while (isPlaying());
}

void SoundPlayer::play(const wxString primary) {
	load(primary);
	play();
}

void SoundPlayer::stop() {
	if (!isPlaying()) {
		logMessage(_T("Sound is already stopped"));
		return;
	}

	logMessage(_T("Stopping sound ..."));

	Mix_HaltChannel(channel);
}

bool SoundPlayer::isLoaded(const wxString filename) {
	// don't allow comparison of empty strings
	if (!primarySound) {
		return false;
	}

	return filename.IsSameAs(primarySound) && isReady();
}

bool SoundPlayer::isReady() {
	return primaryChunk != NULL;
}

bool SoundPlayer::isPlaying() {
	return Mix_Playing(channel) != 0;
}


void initSound() {
	if (initialized) {
		logMessage("Cannot re-initialize sound");
		return;
	}

	soundPlayer = new SoundPlayer();
	soundPlayer->init();
	initialized = true;
}

bool soundIsInitialized() {
	return initialized;
}
