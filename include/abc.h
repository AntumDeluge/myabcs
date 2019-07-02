#ifndef MYABCS_ABC_H
#define MYABCS_ABC_H

#include "resourcelist.h"

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

	void PlayAlphaSound();

	void OnSetCategory(wxCommandEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnTab();

	void OnHelp(wxMenuEvent& event);
	void OnAbout(wxMenuEvent& event);
	void OnFrameFocus(wxFocusEvent& event); // Redirect focus to main panel

	void OnToggleLogWindow(wxMenuEvent& event);
};

#endif /* MYABCS_ABC_H */
