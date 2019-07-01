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


/** checks existence of Vorbis audio file */
wxString prioritizeVorbis(wxString filename) {
	const wxString ogg_filename = filename.Left(filename.Len() - 3).Append("oga");
	if (wxFileExists(ogg_filename)) {
		filename = ogg_filename;
	}

	return filename;
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

	if (primary.IsEmpty()) {
		logError(_T("Cannot load sound from empty filename"));
		return;
	}

	// Vorbis audio takes priority
	primary = prioritizeVorbis(primary);

	if (!wxFileExists(primary)) {
		logError(wxString::Format("Cannot load sound, file not found: %s", primary));
		return;
	}

	if (isLoaded(primary)) {
		logMessage(wxString::Format("Not re-loading sound: %s", primary));
		return;
	}

	primaryChunk = Mix_LoadWAV(primary.c_str());
	if (primaryChunk == NULL) {
		logError(wxString::Format("Unable to load sound file \"%s\": %s", primary, Mix_GetError()));
		return;
	}
	primarySound = primary;

	logMessage(wxString::Format("Loaded sound file: %s", primarySound));
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

void SoundPlayer::play(wxString primary) {
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
