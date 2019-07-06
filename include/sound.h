
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_SOUND_H
#define MYABCS_SOUND_H

#include <SDL2/SDL_mixer.h>
#include <wx/string.h>
#include <wx/window.h>


class SoundPlayer {
public:
	SoundPlayer() {};
	~SoundPlayer();
	void init();
	void load(wxString primary, wxString secondary, wxString auxiliary);
	void load(wxString primary, wxString secondary) { return load(primary, secondary, wxEmptyString); }
	void load(wxString primary) { return load(primary, wxEmptyString, wxEmptyString); }
	void play(wxWindow* source);
	void play(wxWindow* source, wxString primary, wxString secondary, wxString auxiliary);
	void play(wxWindow* source, wxString primary, wxString secondary) { return play(source, primary, secondary, wxEmptyString); }
	void play(wxWindow* source, wxString primary) { return play(source, primary, wxEmptyString, wxEmptyString); }
	void stop();
	bool isLoaded(const wxString filename);
	bool isReady();
	bool isPlaying();
};

extern SoundPlayer* soundPlayer;

extern void initSound();

extern bool soundIsInitialized();

#endif /* MYABCS_SOUND_H */
