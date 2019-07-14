
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "abc.h"
#include "category.h"
#include "event.h"
#include "fonts.h"
#include "gnrcabt.h"
#include "id.h"
#include "paths.h"
#include "sound.h"
#include "res/ani_loading.h"

#include <pthread.h>
#include <wx/mstream.h>
#include <wx/regex.h>
#include <wx/toolbar.h>


static ResourceObject currentResource;
static ResourceObject winResource;

// stored keypress
static int key_pressed = -1;
// determines if letter can be incremented after keypress in "main" mode
static bool alpha_accepted = false;

// event to post when category finishes loading
const wxEventType EVT_CATEGORY_LOADED = wxNewEventType();
static pthread_t category_thread;

// cached objects for category
static ResourceList resourceList;
static int cur_category; // currently loaded category
static bool loading = false;


/** Thread for loading objects used in category
 *
 * TODO: running threads need cleaned up when app exits
 */
void* loadCategoryThread(void* arg) {
	if (resourceList.count() != 0) {
		resourceList.clear();
	}
	resourceList.set(createCategory(cur_category));

	// notify main thread that category finished loading
	wxCommandEvent CatLoadedEvent(EVT_CATEGORY_LOADED);
	wxPostEvent((MainWindow*) arg, CatLoadedEvent);

	return (void*) 1;
}


// checks if a string contains only alphabetic characters
static bool isAlpha(wxString s) {
	return wxRegEx(_T("^[A-Za-z]+$")).Matches(s);
}


MainWindow::MainWindow() :
		wxFrame(NULL, ID_WINDOW, wxEmptyString, wxDefaultPosition, wxSize(400, 550),
				wxDEFAULT_FRAME_STYLE &~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) {
	resourceList = ResourceList();
	winResource = ResourceObject("ribbon", wxEmptyString);

	// Get the executable's filename
	wxString argv0 = getExecutable();

	// FIXME: Currently uses current working directory to locate data
	//        rather than looking in the installed directory.
#ifdef ABCDIR
	installdir = ABCDIR;
	installdir.Append("/");
#else
	installdir = getRootDir();
#endif

	SetIcon(wxIcon(ICON1));

	// TODO: embed toolbar images into binary
	// Tool Bar
	wxBitmap ico_exit(wxImage("data/image/exit.png"));
	wxBitmap ico_abc(wxImage("data/image/abc.png"));
	wxBitmap ico_anim(wxImage("data/image/animals.png"));
	wxBitmap ico_food(wxImage("data/image/food.png"));
	wxBitmap ico_inst(wxImage("data/image/instrument.png"));
	wxBitmap ico_toys(wxImage("data/image/toys.png"));

	menu = CreateToolBar();

	menu->AddRadioTool(ID_ABC, wxEmptyString, ico_abc, wxNullBitmap, wxEmptyString,
			_T("Find the letter on the keyboard"));
	menu->AddRadioTool(ID_FOOD, _T("Food"), ico_food, wxNullBitmap, wxEmptyString,
			_T("Show foods that begin with each letter"));
	menu->AddRadioTool(ID_ANIMALS, _T("Animals"), ico_anim, wxNullBitmap, wxEmptyString,
			_T("Show animals that begin with each letter"));
	menu->AddRadioTool(ID_MUSIC, _T("Music"), ico_inst, wxNullBitmap, wxEmptyString,
			_T("Show musical instruments for each letter"));
	menu->AddRadioTool(ID_TOYS, _T("Toys"), ico_toys, wxNullBitmap, wxEmptyString,
			_T("Show toys for each letter"));

	menu->AddSeparator();
	menu->AddTool(ID_HELP, _T("Help"), wxBitmap(wxImage("data/image/help.png")), wxNullBitmap, wxITEM_NORMAL,
			wxEmptyString, _T("Help"));
	menu->AddTool(wxID_ABOUT, _T("About"), wxBitmap(wxImage("data/image/info.png")), wxNullBitmap, wxITEM_NORMAL,
			wxEmptyString, _T("About MyABCs"));
#ifdef DEBUG
	menu->AddTool(ID_LOG, _T("Log"), wxBitmap(wxImage("data/image/settings.png")), wxNullBitmap, wxITEM_NORMAL,
			wxEmptyString, _T("Show/Hide log window"));
#endif
	menu->AddSeparator();
	menu->AddTool(wxID_EXIT, _T("Exit"), ico_exit, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, _T("Quit"));

	menu->SetBackgroundColour(_T("#84aee6"));
	menu->Realize();

	Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainWindow::OnSetCategory, this);
	Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainWindow::OnHelp, this, ID_HELP);
	Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainWindow::OnAbout, this, wxID_ABOUT);

	// Status bar
	status = new wxStatusBar(this, -1);
	SetStatusBar(status);

	// Background panel
	canvas = new wxPanel(this, ID_BG, wxDefaultPosition, wxDefaultSize, 0 | wxWANTS_CHARS);
	canvas->SetFocus();

	canvas->Bind(wxEVT_KEY_DOWN, &MainWindow::OnKeyDown, this);
	canvas->Bind(wxEVT_KEY_UP, &MainWindow::OnKeyUp, this);

	image = new wxStaticBitmap(canvas, -1, wxNullBitmap);
	letter = new wxStaticText(canvas, -1, _T(""), wxDefaultPosition, wxDefaultSize,
			wxALIGN_CENTRE);
	label = new wxStaticText(canvas, -1, _T(""));
	wxMemoryInputStream is(loading_gif, sizeof(loading_gif));
	wait_image = new wxAnimationCtrl(canvas, -1);
	wait_image->Load(is, wxANIMATION_TYPE_GIF);
	wait_image->Show(false);

	// FIXME: custom fonts only work on Windows platform
	if (font_large.IsOk()) {
		letter->SetFont(font_large);
	} else {
		letter->SetFont(wxFont(45, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	}

	if (font_small.IsOk()) {
		label->SetFont(font_small);
	} else {
		label->SetFont(wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	}

	// Layout
	main_layout = new wxBoxSizer(wxVERTICAL);
	main_layout->AddSpacer(5);
	main_layout->Add(image, 1, wxALIGN_CENTER);
	main_layout->Add(letter, 0, wxALIGN_CENTER);
	main_layout->Add(label, 0, wxALIGN_CENTER);
	main_layout->AddSpacer(10);

	canvas->SetAutoLayout(true);
	canvas->SetSizer(main_layout);
	canvas->Layout();

	Center(); // Center the window on the screen

	// Redirect focus to main panel
	Bind(wxEVT_SET_FOCUS, &MainWindow::OnFrameFocus, this);
	canvas->Bind(wxEVT_KILL_FOCUS, &MainWindow::onLoseFocus, this);
	// sounds finish playing
	Bind(EVT_SOUND_FINISH, &MainWindow::OnSoundFinish, this);
	// category is finished loading
	Bind(EVT_CATEGORY_LOADED, &MainWindow::onCategoryLoaded, this);
	// catch close event to clean up auxiliary threads
	Bind(wxEVT_CLOSE_WINDOW, &MainWindow::onClose, this);

	// logging
	logWindow = new LogWindow(this);
#ifdef DEBUG
	Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainWindow::onToggleLogWindow, this, ID_LOG);
#endif
}

void MainWindow::ReloadDisplay(bool update) {
	// DEBUG:
	logMessage("Reloading display ...");

	image->SetBitmap(currentResource.getBitmap());

	if (update) {
		wxString l = currentResource.getLabel().Upper();
		letter->SetLabel(l[0]);
		label->SetLabel(l);
	}

	canvas->SetFocusIgnoringChildren();
	canvas->Refresh();
	canvas->Layout();
}

void MainWindow::setCategory(const int id) {
	if (loading) {
		logMessage("Please wait for current category to finish loading");
		return;
	}

	// DEBUG:
	logMessage(wxString("Loading category: ").Append(getCategoryName(id)));

	loading = true;
	cur_category = id;
	// start background thread to load category objects
	int t_ret = pthread_create(&category_thread, NULL, loadCategoryThread, this);
	if (t_ret != 0) {
		setErrorCode(t_ret);
		setErrorMsg(wxString("Unknown error creating thread"));
		logCurrentError();
	}
	startWaitAnimation();
}

void MainWindow::SetLetter(wxString alpha) {
	// DEBUG:
	logMessage("Changing letter ...");

	currentResource = resourceList.getObject(alpha);
	ReloadDisplay();
}

void MainWindow::IncrementLetter(wxString alpha, bool advance) {
	// DEBUG:
	logMessage("Incrementing letter ...");

	if (advance) currentResource = resourceList.getNext(alpha);
	else currentResource = resourceList.getPrev(alpha);
	ReloadDisplay();
}

void MainWindow::SetGameEnd(bool play_sound) {
	// DEBUG:
	logMessage("End of game");

	game_end = true;

	currentResource = winResource;
	letter->SetLabel(_T("CONGRATS!"));
	label->SetLabel(_T("Press \"ENTER\" to Play Again"));
	ReloadDisplay(false);
	if (play_sound) currentResource.playSound(this);
}

void MainWindow::PlayAlphaSound() {
	wxString basename = wxString::Format("alpha/%s", getCurrentLetter());
	wxString s_primary = getSoundPath(basename);

	if (s_primary.IsEmpty()) {
		logError(wxString::Format("Sound file not found: %s/%s", getRootDir(), basename));

		// need to flush pressed key state because error message prevents main thread from catching key release event
		key_pressed = -1;
		return;
	}

	if (cur_category == ID_ABC) {
		soundPlayer->play(this, s_primary, currentResource.getVocalString(), currentResource.getEffectString());
	} else {
		soundPlayer->play(this, s_primary);
	}
}

void MainWindow::PlayResourceSound() {
	wxChar object_letter = currentResource.getLabel().Lower()[0]; // FIXME: need failsafe in case return is empty string
	wxString current_letter = getCurrentLetter().Lower();

	// don't play sound if on category title
	if (object_letter == current_letter) {
		currentResource.playSound(this);
	}
}

void MainWindow::OnSetCategory(wxCommandEvent& event) {
	if (loading) {
		// don't change selected toolbar button
		menu->ToggleTool(cur_category, true);
		logMessage("Please wait for current category to finish loading");
		return;
	}

	if (soundIsInitialized()) {
		soundPlayer->stop();
	}

	const int id = event.GetId();

	if (id == wxID_EXIT) {
		Close(true);
	} else {
		setCategory(id);
	}
}

void MainWindow::onCategoryLoaded(wxEvent& event) {
	loading = false;
	stopWaitAnimation();

	if (cur_category == ID_ANIMALS) {
		SetTitle("Press a Key to See an Animal");
		currentResource = resourceList.getObject("t");
		letter->SetLabel("Animals");
	} else if (cur_category == ID_MUSIC) {
		SetTitle("Press a Key to See an Instrument");
		currentResource = resourceList.getObject("g");
		letter->SetLabel("Music");
	} else if (cur_category == ID_FOOD) {
		SetTitle("Press a Key to See a Food");
		currentResource = resourceList.getObject("h");
		letter->SetLabel("Food");
	} else if (cur_category == ID_TOYS) {
		SetTitle("Press a Key to See a Toy");
		currentResource = resourceList.getObject("w");
		letter->SetLabel("Toys");
	} else {
		currentResource = resourceList.getObject("a");
		SetTitle("Find the letter on the keyboard");
		ReloadDisplay(true);

		return;
	}

	label->SetLabel("PRESS A KEY ON THE KEYBOARD");
	ReloadDisplay(false);
	game_end = false;
}

// FIXME: if sound fails to play, space key release isn't caught (still valid?)
void MainWindow::OnKeyDown(wxKeyEvent& event) {
	// TODO: failsafe check for wxKeyEvent

	if (key_pressed != -1) {
		logMessage(wxString::Format("WARNING: \"%c\" key has not been released", wxChar(key_pressed)));
	}

	if (!loading && key_pressed == -1) { // -1 means stored keypress has been released
		const int key_code = event.GetKeyCode();
		wxChar key = wxChar(key_code);
		// store key pressed state
		key_pressed = key_code;

		if (key_code == WXK_TAB) {
			handleKeyTab();
		} else if (key_code == WXK_SPACE) {
			handleKeySpace();
		} else if (key_code == WXK_LEFT || key_code == WXK_RIGHT || key_code == WXK_BACK) {
			handleKeyArrow(key_code);
		} else if (key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER) {
			handleKeyEnter();
		} else if (isAlpha(key) && !soundPlayer->isPlaying()) {
			// DEBUG:
			logMessage(wxString::Format("Key down: %c", key));

			if (cur_category == ID_ABC) {
				handleKeyAlphaMain(key);
			} else {
				handleKeyAlphaOther(key);
			}
		}
	}

	event.Skip();
}

void MainWindow::OnKeyUp(wxKeyEvent& event) {
	// TODO: failsafe check for wxKeyEvent
	const int key_code = event.GetKeyCode();

	if (key_code == key_pressed) {
		// reset key pressed state
		key_pressed = -1;

		// DEBUG:
		logMessage(wxString::Format("%c key up ...", wxChar(key_code)));
	}

	event.Skip();
}

void MainWindow::handleKeyTab() {
	if (loading) {
		// not necessary, but prevents extra tool toggle when tab button pressed
		logMessage("Please wait for current category to finish loading");
		return;
	}

	// Changes game modes via the "Tab" key
	int tools[] = {ID_ABC, ID_FOOD, ID_ANIMALS, ID_MUSIC, ID_TOYS};
	for (int tool = 4; tool >= 0; tool -= 1) {
		if (menu->GetToolState(tools[tool])) {
			if (tools[tool] == ID_TOYS) {
				menu->ToggleTool(tools[0], true);
				wxMenuEvent changemode(wxEVT_COMMAND_MENU_SELECTED, tools[0]);
				wxPostEvent(this, changemode);
				break;
			} else {
				menu->ToggleTool(tools[tool + 1], true);
				wxMenuEvent changemode(wxEVT_COMMAND_MENU_SELECTED, tools[tool + 1]);
				wxPostEvent(this, changemode);
			}
		}
	}
}

void MainWindow::handleKeySpace() {
	if (!soundPlayer->isPlaying()) {
		PlayResourceSound();
	}
}

void MainWindow::handleKeyArrow(const int key_code) {
	// ignore backspace key for categories other than "main"
	if (cur_category != ID_ABC) return;

	const bool back = key_code == WXK_LEFT || key_code == WXK_BACK;
	bool forward = false;

#ifdef DEBUG
	forward = key_code == WXK_RIGHT;
#endif

	if (back) {
		if (getCurrentLetter() != "A") {
			if (game_end) {
				if (soundPlayer->isPlaying()) soundPlayer->stop();
				SetLetter("Z");
				game_end = false;
			} else if (!soundPlayer->isPlaying()) {
				IncrementLetter(false);
			}
		}
	} else if (forward) {
		if (!game_end && getCurrentLetter() != "Z") {
			if (!soundPlayer->isPlaying()) {
				IncrementLetter();
			}
		}
	}
}

void MainWindow::handleKeyEnter() {
	if (cur_category == ID_ABC && game_end) {
		// DEBUG:
		logMessage("Restarting game ...");

		if (soundPlayer->isPlaying()) soundPlayer->stop();
		SetLetter("A");
		game_end = false;
	}
}

void MainWindow::handleKeyAlphaMain(wxChar alpha) {
	if (alpha == getCurrentLetter()) {
		alpha_accepted = true; // letter will be incremented after sound plays
		PlayAlphaSound();
	}
}

void MainWindow::handleKeyAlphaOther(wxChar alpha) {
	// don't refresh display if letter does't change
	if (alpha != getCurrentLetter()) SetLetter(alpha);
	PlayAlphaSound();
}

void MainWindow::OnSoundFinish(wxEvent& event) {
	if (cur_category == ID_ABC) {
		if (alpha_accepted) {
			if (getCurrentLetter() == "Z") {
				SetGameEnd();
			} else {
				IncrementLetter();
			}
			alpha_accepted = false; // reset accepted keypress state
		}
	}
}

void MainWindow::startWaitAnimation() {
	letter->SetLabel("Loading...");
	label->SetLabel(wxEmptyString);
	image->Show(false);
	main_layout->Detach(1);
	main_layout->Insert(1, wait_image, 1, wxALIGN_CENTER); // FIXME: not centered vertically
	wait_image->Show();
	wait_image->Play();

	canvas->Layout();
}

void MainWindow::stopWaitAnimation() {
	letter->SetLabel(wxEmptyString);
	label->SetLabel(wxEmptyString);
	wait_image->Stop();
	wait_image->Show(false);
	main_layout->Detach(1);
	main_layout->Insert(1, image, 1, wxALIGN_CENTER);
	image->Show();

	// no need to call canvas->Layout() here because MainWindow::ReloadDisplay will be called
}

// Help and About dialogs

void MainWindow::OnHelp(wxCommandEvent& event) {
	wxDialog *help = new wxDialog(this, -1, _T("Help"), wxDefaultPosition,
			wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

#ifdef WIN32
	help->SetIcon(ICON1);
#else
	help->SetSize(wxSize(450, 250));
#endif

	wxRichTextCtrl *textarea = new wxRichTextCtrl(help, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY);
	wxButton *ok = new wxButton(help, wxID_OK);

	textarea->Newline();

	textarea->BeginAlignment(wxTEXT_ALIGNMENT_CENTER);
	textarea->BeginFontSize(14);
	textarea->BeginBold();
	textarea->WriteText("HOW TO PLAY\n");
	textarea->EndAllStyles();

	textarea->BeginBold();
	textarea->BeginUnderline();
	textarea->WriteText("\nSelecting a category:\n");
	textarea->EndAllStyles();
	textarea->BeginLeftIndent(50);
	textarea->WriteText("\
The four icons on the left side of the toolbar represent the \
different modes.  Use the \"Tab\" key to toggle between modes, \
or click on an icon with the mouse.\n");
	textarea->EndLeftIndent();

	textarea->BeginBold();
	textarea->BeginUnderline();
	textarea->WriteText("\nAlphabet:\n");
	textarea->EndAllStyles();
	textarea->BeginLeftIndent(50);
	textarea->WriteText("\
The first icon on the left is the Alphabet Mode.  Press the \
letter on the keyboard that is displayed on the screen to \
cycle through the English alphabet.  Press the \"Backspace\" \
key to move back one letter.  Finish by finding all the \
letters, A-Z.\n");
	textarea->EndLeftIndent();

	textarea->BeginBold();
	textarea->BeginUnderline();
	textarea->WriteText("\nOther categories:\n");
	textarea->EndAllStyles();
	textarea->BeginLeftIndent(50);
	textarea->WriteText("\
In all other modes, simply press a key on the keyboard to see \
a letter with a related image and description.\n");
	textarea->EndLeftIndent();

	textarea->BeginBold();
	textarea->BeginUnderline();
	textarea->WriteText("\nSounds:\n");
	textarea->EndAllStyles();
	textarea->BeginLeftIndent(50);
	textarea->WriteText("\
Press the spacebar to hear the name of the pictured object.\n");
	textarea->EndLeftIndent();

	wxBoxSizer *help_sizer = new wxBoxSizer(wxVERTICAL);
	help_sizer->Add(textarea, 1, wxEXPAND);
	help_sizer->Add(ok, 0, wxALIGN_CENTER);

	help->SetAutoLayout(true);
	help->SetSizer(help_sizer);
	help->Layout();
	help->CenterOnParent();
	help->ShowModal();
}

void MainWindow::OnAbout(wxCommandEvent& event) {
	showAboutDialog(this);
}

void MainWindow::OnFrameFocus(wxFocusEvent& event) {
	canvas->SetFocus();
}

void MainWindow::onLoseFocus(wxFocusEvent& event) {
	// DEBUG:
	logMessage("Focus lost, flushing pressed key state...");

	key_pressed = -1;
}

void MainWindow::onToggleLogWindow(wxCommandEvent& event) {
	logWindow->Show(!logWindow->IsShown());
}

void MainWindow::onClose(wxCloseEvent& event) {
	// DEBUG:
	logMessage("Exiting program ...");

	// FIXME: segfault occurs if log messages appear while shutting down
	disableLogging();

	// FIXME: appears to be a delay when frame "X" is pressed
	// clean up sound thread
	if (soundPlayer->isPlaying()) {
		soundPlayer->stop();
	}

	// clean up category loading thread
	if (loading) {
		int t_ret = pthread_cancel(category_thread);
		if (t_ret != 0) {
			setError(t_ret, "Error cancelling sound thread");
			logCurrentError();
		} else {
			logMessage("Category loading thread exited cleanly");
		}
	}

	// XXX: would it be good to call pthread_join here?

	Destroy();
}
