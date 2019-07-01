#ifndef MYABCS_ABC_H
#define MYABCS_ABC_H

#include "imgdisplay.h"
#include "res.h"
#include "resourcelist.h"
#include "resourceobject.h"

#include <wx/wx.h>
#include <wx/sound.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <pthread.h>
#include <fstream>


class MainWindow : public wxFrame {
public:
	MainWindow();
	void LoadCategory(wxString cat_name);
	void GoABC();
	void GoOther();
	void ChangeLetter(wxCommandEvent& event);
	void PlaySound();
	void EnableAll(wxCommandEvent& event);
	void SetCurSound(wxString wav);
private:
	// main GUI elements
	wxToolBar *menu;
	wxStatusBar *status;

	// game UI
	wxPanel* canvas;
	ImageDisplay* image;
	wxStaticText* letter;
	wxStaticText* label;

	// directories
	wxString installdir; // Directory where files should be installed

	char key; // Store the key pressed
	char cur_letter;
	static wxString alpha_sound;
	wxString letter_sounds[2][26]; // wav sound for each letter
	wxString images[26];
	wxString letters[26];
	wxString labels[26];
	wxString foods[2][26];
	wxString animals[2][26];
	wxString musics[2][26];
	wxString toys[2][26];
	//    wxButton *play; // Button to play sounds
	bool gameend;
	bool isplaying; // Is sound currently playing
	bool canspace; // Allow playing sounds if one is not currently played
	bool cankey;

	// cached objects for category
	ResourceList resourceList;

	// Help Dialog
	//wxDialog *help;

	// About Dialog
	//GenericAbout *about;

	// Threading
	//static void *TabThread(void *arg);
	static void* SpaceThread(void* arg);
	static void* KeyThread(void* arg);
	static void* OtherThread(void* arg);
	pthread_t thread1;
	pthread_t thread2;
	int rc;

	// methods

	wxString getCurrentLetter() { return letter->GetLabel(); }

	void ReloadDisplay(bool update);
	void ReloadDisplay() { return ReloadDisplay(true); }
	void SetLetter(wxString alpha);
	void IncrementLetter(wxString alpha, bool advance);
	void IncrementLetter(wxString alpha) { return IncrementLetter(alpha, true); }
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
