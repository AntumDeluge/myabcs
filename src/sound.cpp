#include <SDL2/SDL.h>
#include <wx/filefn.h>

#include "log.h"
#include "sound.h"


static Mix_Chunk* chunk = NULL;
static int channel;
static wxString loadedSound;

// check for sound intializations
bool initialized = false;


SoundPlayer::SoundPlayer() {
	loadedSound = wxEmptyString;
}

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

void SoundPlayer::load(wxString filename) {
	// Vorbis audio takes priority
	const wxString ogg_filename = filename.Left(filename.Len() - 3).Append("oga");
	if (wxFileExists(ogg_filename)) {
		filename = ogg_filename;
	}

	if (isLoaded(filename)) {
		logMessage(wxString("Not re-loading sound: ").Append(filename));
		return;
	}

	// ensure chunk is NULL every time this method is called
	unload();

	if (filename.IsEmpty()) {
		logError(_T("Cannot load sound from empty filename"));
		return;
	}

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

	loadedSound = filename;

	logMessage(wxString("Loaded sound file: ").Append(filename));
}

void SoundPlayer::load(const std::string filename) {
	return load(wxString(filename));
}

void SoundPlayer::unload() {
	Mix_FreeChunk(chunk);
	chunk = NULL;
	loadedSound.Empty();
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

void SoundPlayer::play(const wxString filename) {
	load(filename);
	play();
}

void SoundPlayer::play(const std::string filename) {
	load(filename);
	play();
}

void SoundPlayer::stop() {
	if (!isPlaying()) {
		logMessage(_T("Sound is already stopped"));
		return;
	}

	Mix_HaltChannel(channel);

	logMessage(_T("Stopped playing sound"));
}

bool SoundPlayer::isLoaded(const wxString filename) {
	// don't allow comparison of empty strings
	if (!loadedSound) {
		return false;
	}

	return filename.IsSameAs(loadedSound) && isReady();
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
	initialized = true;
}

bool soundIsInitialized() {
	return initialized;
}
