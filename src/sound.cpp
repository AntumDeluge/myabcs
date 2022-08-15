
/* Copyright © 2010-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "event.h"
#include "log.h"
#include "sound.h"

#include <pthread.h>
#include <SDL2/SDL.h>
#include <wx/filefn.h>


SoundPlayer* soundPlayer;

// check for sound intializations
static bool initialized = false;

static Mix_Chunk* primaryChunk = NULL;
static Mix_Chunk* secondaryChunk = NULL;
static Mix_Chunk* auxiliaryChunk = NULL;
static int channel;
static wxString primarySound;
static wxString secondarySound;
static wxString auxiliarySound;

// thread object
static pthread_t sound_thread;
bool thread_is_active = false;


void unloadChunks() {
	// XXX: leak?
	primaryChunk = NULL;
	secondaryChunk = NULL;
	auxiliaryChunk = NULL;
	primarySound.Empty();
	secondarySound.Empty();
	auxiliarySound.Empty();
}


/** cleans up for thread exit */
static void onExitSoundThread(void* arg) {
	unloadChunks();
	thread_is_active = false;

	wxEvtHandler* source_window = wxDynamicCast(arg, wxEvtHandler);
	if (source_window) {
		// DEBUG:
		logMessage("Sending SoundFinishEvent ...");

		// event to send to main thread
		wxCommandEvent SoundFinishEvent(EVT_SOUND_FINISH);
		wxPostEvent(source_window, SoundFinishEvent);
	}
}


// thread for playing sounds
void* playSoundThread(void* arg) {
	thread_is_active = true;

	channel = Mix_PlayChannel(-1, primaryChunk, 0);
	if (channel != 0) {
		logError(wxString::Format("Playing primary sound failed: %s", Mix_GetError()));
		onExitSoundThread(arg);
		return (void*) 1;
	}

	logMessage(wxString("Playing sound ..."));

	// wait for sound to stop playing
	while (Mix_Playing(channel) != 0);

	// TODO: short pause between sounds
	if (secondaryChunk != NULL) {
		channel = Mix_PlayChannel(-1, secondaryChunk, 0);
		if (channel != 0) {
			logError(wxString::Format("Playing secondary sound failed: %s", Mix_GetError()));
			onExitSoundThread(arg);
			return (void*) 1;
		}

		while (Mix_Playing(channel) != 0);
	}

	if (auxiliaryChunk != NULL) {
		channel = Mix_PlayChannel(-1, auxiliaryChunk, 0);
		if (channel != 0) {
			logError(wxString::Format("Playing auxiliary sound failed: %s", Mix_GetError()));
			onExitSoundThread(arg);
			return (void*) 1;
		}

		while (Mix_Playing(channel) != 0);
	}

	onExitSoundThread(arg);
	return (void*) 0;
}


/** checks existence of Vorbis audio file */
wxString prioritizeVorbis(wxString filename) {
	// FIXME: need more failsafe way to check filenames
	// Replaces .flac file exstension with .oga
	const wxString ogg_filename = filename.Left(filename.Len() - 4).Append("oga");
	if (wxFileExists(ogg_filename)) {
		filename = ogg_filename;
	}

	return filename;
}


SoundPlayer::~SoundPlayer() {
	if (isPlaying()) {
		stop();
	}

	Mix_FreeChunk(primaryChunk);
	Mix_FreeChunk(secondaryChunk);
	Mix_FreeChunk(auxiliaryChunk);
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

void SoundPlayer::load(wxString primary, wxString secondary, wxString auxiliary) {
	// ensure sound chunks are NULL every time this method is called
	unloadChunks();

	if (primary.IsEmpty()) {
		logError(_T("Cannot load sound from empty filename"));
		return;
	}

	// Vorbis audio takes priority
	primary = prioritizeVorbis(primary);
	secondary = prioritizeVorbis(secondary);
	auxiliary = prioritizeVorbis(auxiliary);

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

	if (wxFileExists(secondary)) {
		secondaryChunk = Mix_LoadWAV(secondary.c_str());
		secondarySound = secondary;
	}

	if (wxFileExists(auxiliary)) {
		auxiliaryChunk = Mix_LoadWAV(auxiliary.c_str());
		auxiliarySound = auxiliary;
	}

	logMessage(wxString::Format("Loaded sound file: %s", primarySound));
}

void SoundPlayer::play(wxWindow* source) {
	if (!isReady()) {
		logError(_T("Audio not loaded, cannot play sound"));
		return;
	}

	// don't play if sound is already playing
	if (isPlaying()) {
		logMessage(_T("Audio channel not available"));
		return;
	}

	// thread function plays the sound
	int t_ret = pthread_create(&sound_thread, NULL, playSoundThread, source);
	if (t_ret != 0) {
		setErrorCode(t_ret);
		setErrorMsg(wxString("Unknown error creating thread"));
		logCurrentError();

		return;
	}
}

void SoundPlayer::play(wxWindow* source, wxString primary, wxString secondary, wxString auxiliary) {
	load(primary, secondary, auxiliary);
	play(source);
}

void SoundPlayer::stop() {
	if (!isPlaying()) {
		logMessage(_T("Sound is already stopped"));
		return;
	}

	Mix_HaltChannel(channel);

	// XXX: necessary even after Mix_HaltChannel called?
	int t_ret = pthread_cancel(sound_thread);
	if (t_ret != 0) {
		setError(t_ret, "Error cancelling sound thread");
		logCurrentError();

		return;
	} else {
		logMessage("Sound thread exited cleanly");
	}

	thread_is_active = false;
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
	return thread_is_active || (Mix_Playing(channel) != 0);
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
