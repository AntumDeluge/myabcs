#include <wx/wx.h>
#include <wx/sound.h>
#include <wx/aboutdlg.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <pthread.h>
#include "res.h"
#include "gnrcabt.h"
#include <fstream>
using namespace std;

const int ID_EXIT = wxNewId();
const int ID_ABC = wxNewId();
const int ID_FOOD = wxNewId();
const int ID_ANIMALS = wxNewId();
const int ID_MUSIC = wxNewId();
const int ID_TOYS = wxNewId();
const int ID_HELP = wxNewId();
const int ID_ABOUT = wxNewId();
const int ID_SPACE = wxNewId(); // Id used in threads for space
//const int ID_TAB = wxNewId(); // Id used in threads for tab
const int ID_KEY = wxNewId(); // Id used in threads for keypress
const int ID_OTHER = wxNewId(); // Id used in other threads

class MainWindow : public wxFrame
{
  public:
    MainWindow(const wxString& title);
    void SetMode(wxCommandEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnTab();//wxCommandEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void GoABC();
    void GoOther();
    void ChangeLetter(wxCommandEvent& event);
    void PlaySound();
	void OnHelp(wxMenuEvent& event);
	void OnAbout(wxMenuEvent& event);
	void EnableAll(wxCommandEvent& event);
	void SetCurSound(wxString wav);
	void OnFrameFocus(wxFocusEvent& event); // Redirect focus to main panel
  private:
    wxString *exedir; // Directory where executable is located
    wxToolBar *menu;
	wxStatusBar *status;
    wxPanel *bg;
    char key; // Store the key pressed
    char cur_letter;
    static wxString cur_sound;
    wxString letter_sounds[2][26]; // wav sound for each letter
    wxStaticBitmap *image;
    wxString images[26];
    wxStaticText *letter;
    wxString letters[26];
    wxStaticText *label;
    wxString labels[26];
    wxString foods[2][26];
    wxString animals[2][26];
    wxString musics[2][26];
    wxString toys[2][26];
//    wxButton *play; // Button to play sounds
    wxSound *cheer;
    bool gameend;
    bool isplaying; // Is sound currently playing
    bool canspace; // Allow playing sounds if one is not currently played
    bool cantab; // Allow switching modes with tab button
    bool cankey;

    // Help Dialog
    wxDialog *help;

    // About Dialog
    GenericAbout *about;

    // Threading
    //static void *TabThread(void *arg);
    static void *SpaceThread(void *arg);
    static void *KeyThread(void *arg);
	static void *OtherThread(void *arg);
    pthread_t thread1;
    pthread_t thread2;
    int rc;
};
