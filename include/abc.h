
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_ABC_H
#define MYABCS_ABC_H

#include "resourcelist.h"

#include <wx/animate.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>


class MainWindow : public wxFrame {
public:
	MainWindow();
	void setCategory(const int id);
private:
	wxBoxSizer* main_layout;

	// main GUI elements
	wxToolBar *menu;
	wxStatusBar *status;

	// game UI
	wxPanel* canvas;
	wxStaticBitmap* image;
	wxStaticText* letter;
	wxStaticText* label;
	wxAnimationCtrl* wait_image; // animation used while category is loading

	// directories
	wxString installdir; // Directory where files should be installed

	// game state
	bool game_end;

	// methods

	wxString getCurrentLetter() { return letter->GetLabel(); }

	void ReloadDisplay(bool update);
	void ReloadDisplay() { return ReloadDisplay(true); }
	void SetLetter(wxString alpha);
	void IncrementLetter(wxString alpha, bool advance);
	void IncrementLetter(wxString alpha) { return IncrementLetter(alpha, true); }
	void IncrementLetter(bool advance) { return IncrementLetter(getCurrentLetter(), advance); }
	void IncrementLetter() { return IncrementLetter(getCurrentLetter(), true); }
	void SetGameEnd(bool play_sound);
	void SetGameEnd() { return SetGameEnd(true); }

	void PlayAlphaSound();
	void PlayResourceSound();

	void OnSetCategory(wxCommandEvent& event);
	void onCategoryLoaded(wxEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void handleKeyTab();
	void handleKeySpace();
	void handleKeyArrow(const int key_code);
	void handleKeyEnter();
	void handleKeyAlphaMain(wxChar alpha);
	void handleKeyAlphaOther(wxChar alpha);

	void OnSoundFinish(wxEvent& event);

	void startWaitAnimation();
	void stopWaitAnimation();

	void OnHelp(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnFrameFocus(wxFocusEvent& event); // Redirect focus to main panel

	void OnToggleLogWindow(wxCommandEvent& event);
};

#endif /* MYABCS_ABC_H */
