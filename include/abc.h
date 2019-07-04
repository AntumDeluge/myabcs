#ifndef MYABCS_ABC_H
#define MYABCS_ABC_H

#include "resourcelist.h"

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>


class MainWindow : public wxFrame {
public:
	MainWindow();
	void LoadCategory(wxString cat_name);
private:
	// main GUI elements
	wxToolBar *menu;
	wxStatusBar *status;

	// game UI
	wxPanel* canvas;
	wxStaticBitmap* image;
	wxStaticText* letter;
	wxStaticText* label;

	// directories
	wxString installdir; // Directory where files should be installed

	// game state
	wxString cur_category; // currently loaded category
	bool game_end;

	// cached objects for category
	ResourceList resourceList;

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

	void OnSetCategory(wxCommandEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void handleKeyTab();
	void handleKeySpace();
	void handleKeyBack();
	void handleKeyEnter();
	void handleKeyAlphaMain(wxChar alpha);
	void handleKeyAlphaOther(wxChar alpha);

	void OnSoundFinish(wxEvent& event);

	void OnHelp(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnFrameFocus(wxFocusEvent& event); // Redirect focus to main panel

	void OnToggleLogWindow(wxCommandEvent& event);
};

#endif /* MYABCS_ABC_H */
