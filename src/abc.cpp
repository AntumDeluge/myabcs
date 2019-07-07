
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
#include "log.h"
#include "paths.h"
#include "sound.h"
#include "res/ani_loading.h"
#include "res/logo.h"

#include <pthread.h>
#include <wx/ffile.h>
#include <wx/mstream.h>
#include <wx/regex.h>
#include <wx/toolbar.h>


// icon
#if !defined(WIN32) && !defined(WIN64)
#define ICON1 _T("myabcs.png")
#else
#define ID_ICON1
#define ICON1 wxICON(ID_ICON1)
#endif


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
#ifdef DEBUG
	Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainWindow::OnToggleLogWindow, this, ID_LOG);
#endif

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
	// sounds finish playing
	Bind(EVT_SOUND_FINISH, &MainWindow::OnSoundFinish, this);
	// category is finished loading
	Bind(EVT_CATEGORY_LOADED, &MainWindow::onCategoryLoaded, this);
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
	wxString s_primary = getSoundFile(wxString::Format("alpha/%s", getCurrentLetter()));

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

	label->SetLabel(wxEmptyString);
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

	if (key_code == WXK_LEFT || key_code == WXK_BACK) {
		if (getCurrentLetter() != "A") {
			if (game_end) {
				if (soundPlayer->isPlaying()) soundPlayer->stop();
				SetLetter("Z");
				game_end = false;
			} else if (!soundPlayer->isPlaying()) {
				IncrementLetter(false);
			}
		}
	} else {
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

	wxString helptext(_T("How to use:\n\
Selecting a mode:\n\
	The four icons on the left side of the toolbar represent the\n\
	different modes.  Use the \"Tab\" key to toggle between modes,\n\
	or click on an icon with the mouse.\n\
\n\
Alphabet Mode:\n\
	The first icon on the left is the Alphabet Mode.  Press the\n\
	letter on the keyboard that is displayed on the screen to\n\
	cycle through the English alphabet.  Press the \"Backspace\"\n\
	key to move back one letter.  Finish by finding all the\n\
	letters, A-Z.\n\
\n\
Other Modes:\n\
	In all other modes, simply press a key on the keyboard to see\n\
	a letter with a related image and description.\n\
\n\
Sounds:\n\
	Press the spacebar to hear the name of the pictured object."));

	textarea->SetValue(helptext);

	wxBoxSizer *help_sizer = new wxBoxSizer(wxVERTICAL);
	help_sizer->Add(textarea, 1, wxEXPAND);
	help_sizer->Add(ok, 0, wxALIGN_CENTER);

	help->SetAutoLayout(true);
	help->SetSizer(help_sizer);
	help->Layout();
	help->ShowModal();

	help->CenterOnParent();
}

void MainWindow::OnAbout(wxCommandEvent& event) {
	// About Dialog
	GenericAbout* about = new GenericAbout(this, -1);
	about->SetIcon(wxIcon(ICON1));
	wxSize image_size = about->getImageSize();
	about->setImage(imageFromSVG((unsigned char*) myabcs_svg, sizeof(myabcs_svg), image_size.GetWidth(), image_size.GetHeight()), true);

	wxString version_string = wxEmptyString;
#ifdef VERSION
	version_string = VERSION;
#ifdef VER_DEV
	version_string.Append("-dev").Append(VER_DEV);
#endif
#endif

	wxString info_string = "MyABCs";
	if (!version_string.IsEmpty()) {
		info_string.Append(wxString::Format(" %s", version_string));
	}
	about->setInfoString(info_string);

	about->setCopyright(_T("\u00A9 Jordan Irwin 2010-2019"));
	about->setLink("myabcs.sourceforge.io", "https://myabcs.sourceforge.io/");
	about->setAbout("MyABCs is educational software for young children to learn \nthe English alphabet and get familiar with a keyboard");

	// Changelog
	// FIXME: concatenating "CHANGES.txt" to dir_root not working
	//wxString cl_file = wxString::Format("%s/CHANGES.txt", getRootDir());
	wxString cl_file = _T("CHANGES.txt");
	wxString cl_text = wxString::Format("Changelog text not found: %s", cl_file);

	if (wxFileExists(cl_file)) {
		// load text file
		wxFFile f_open(cl_file, "r");
		f_open.ReadAll(&cl_text);
		f_open.Close();
	}

	about->setChangelog(cl_text);

	about->addArtist("ABC Blocks", "Petri Lummemaki", "Public Domain");
	about->addArtist("Accordion", "ArtFavor", "Public Domain");
	about->addArtist("Airplane", "Jarno Vasamaa", "Public Domain");
	about->addArtist("Anklet", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98184
	about->addArtist("Angelfish", "Jonathon Love", "Public Domain");
	about->addArtist("Apple", "Chrisdesign", "Public Domain");
	about->addArtist("Bagpipes", "Jordan Irwin (derivative)", "Public Domain"); // https://opengameart.org/node/98175
	about->addArtist("Balloons", "AJ", "Public Domain");
	about->addArtist("Bananas", "nicubunu", "Public Domain");
	about->addArtist("Bird", "ArtFavor", "Public Domain");
	about->addArtist("Bicycle", "alvolk", "Public Domain");
	about->addArtist("Cat", "Francesco Rollandin", "Public Domain");
	about->addArtist("Chalkboard", "J_Alves", "Public Domain");
	about->addArtist("Cherries", "Rocket000", "Public Domain");
	about->addArtist("Clarinet", "Gerald_G", "Public Domain");
	about->addArtist("Clock", "Jonathan Dietrich", "Public Domain");
	about->addArtist("Dog", "Gerald_G", "Public Domain");
	about->addArtist("Doll", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98192
	about->addArtist("Dolphin", "Andrew Fitzsimon", "Public Domain");
	about->addArtist("Doughnut", "worms_x", "Public Domain");
	about->addArtist("Drums", "TheresaKnott", "Public Domain");
	about->addArtist("Earth", "Dan Gerhrads", "Public Domain");
	about->addArtist("Easel", "Gerald_G", "Public Domain");
	about->addArtist("Egg", "dStulle", "Public Domain");
	about->addArtist("Electric Guitar", "Chrisdesign", "Public Domain");
	about->addArtist("Elephant", "ArtFavor", "Public Domain");
	about->addArtist("Fire", "Valessio Soares de Brito", "Public Domain");
	about->addArtist("Flute", "Gerald_G", "Public Domain");
	about->addArtist("Football", "molumen", "Public Domain");
	about->addArtist("Fries", "Juliane Krug", "Public Domain");
	about->addArtist("Frog", "leland_mcinnes", "Public Domain");
	about->addArtist("Giraffe", "Packard Jennings", "Public Domain");
	about->addArtist("Glove", "Gerald_G", "Public Domain");
	about->addArtist("Grapes", "Jean-Victor Balin", "Public Domain");
	about->addArtist("Grasshopper", "Francesco Rollandin", "Public Domain");
	about->addArtist("Guitar", "papapishu", "Public Domain");
	about->addArtist("Harp", "Gerald_G", "Public Domain");
	about->addArtist("Hat", "Gerald_G", "Public Domain");
	about->addArtist("Horn", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98191
	about->addArtist("Horse", "ArtFavor", "Public Domain");
	about->addArtist("Hot Dog", "Juliane Krug", "Public Domain");
	about->addArtist("Ice Cream", "Gerald_G", "Public Domain");
	about->addArtist("Ice Skate", "Francesco Rollandin", "Public Domain");
	about->addArtist("Igloo", "Jose Hevia", "Public Domain");
	about->addArtist("Iguana", wxEmptyString, "Public Domain");
	about->addArtist("Ipu", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98176
	about->addArtist("Jaguar", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98089
	about->addArtist("Jar", "DTRave", "Public Domain");
	about->addArtist("Jug", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98177
	about->addArtist("Juice", "Gerald_G", "Public Domain");
	about->addArtist("Jump Rope", "johnny_automatic", "Public Domain");
	about->addArtist("Kazoo", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98178
	about->addArtist("Ketchup", "Alexandre Norman", "Public Domain");
	about->addArtist("Keyboard", "yeKcim", "Public Domain");
	about->addArtist("Kite", "schoolfreeware", "Public Domain");
	about->addArtist("Koala", "ArtFavor", "Public Domain");
	about->addArtist("Ladybug", "lalolalo", "Public Domain");
	about->addArtist("Lasagna", "Gerald_G", "Public Domain");
	about->addArtist("Lion", "Francesco Rollandin", "Public Domain");
	about->addArtist("Lunchbox", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98190
	about->addArtist("Lute", "papapishu", "Public Domain");
	about->addArtist("Magnet", "Francesco Rollandin", "Public Domain");
	about->addArtist("Magnifying Glass", "TheStructorr", "Public Domain");
	about->addArtist("Maracas", "Gerald_G", "Public Domain");
	about->addArtist("Mouse", "ArtFavor", "Public Domain");
	about->addArtist("Mushroom", "Tanguy JACQ", "Public Domain");
	about->addArtist("Necklace", "wsnaccad", "Public Domain");
	about->addArtist("Newspaper", "Aubanel Monnier", "Public Domain");
	about->addArtist("Ney", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98180
	about->addArtist("Newt", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98524
	about->addArtist("Noodle", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98168
	about->addArtist("Oboe", "Gerald_G", "Public Domain");
	about->addArtist("Olive", "johnny_automatic", "Public Domain");
	about->addArtist("Onion", "Chrisdesign", "Public Domain");
	about->addArtist("Orca", "Matthew Gates", "Public Domain"); // unused
	about->addArtist("Origami", "badaman", "Public Domain");
	about->addArtist("Owl", wxEmptyString, "Public Domain");
	about->addArtist("Paint", "valessiobrito", "Public Domain");
	about->addArtist("Paper Airplane", "nicubunu", "Public Domain");
	about->addArtist("Piano", "ArtFavor", "Public Domain");
	about->addArtist("Pretzel", "Nathan Eady", "Public Domain");
	about->addArtist("Pig", "ArtFavor", "Public Domain");
	about->addArtist("Quail", "ArtFavor", "Public Domain");
	about->addArtist("Quarter", "Paul Sherman", "Public Domain");
	about->addArtist("Quena", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98181
	about->addArtist("Quiche", wxEmptyString, "Public Domain"); // https://www.wpclipart.com/food/meals/quiche.png.html
	about->addArtist("Quilt", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98189
	about->addArtist("Raccoon", "ArtFavor", "Public Domain");
	about->addArtist("Radishes", "Francesco Rollandin", "Public Domain");
	about->addArtist("Recorder", "zeimusu", "Public Domain");
	about->addArtist("Ribbon", wxEmptyString, "CC0/Public Domain"); // https://publicdomainvectors.org/en/free-clipart/Medal-of-achievement-blue-and-red-vector-image/19941.html
	about->addArtist("Rocking Horse", "Chrisdesign", "Public Domain");
	about->addArtist("Rooster", "ArtFavor", "Public Domain");
	about->addArtist("Saxophone", "ArtFavor", "Public Domain");
	about->addArtist("Seesaw", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98188
	about->addArtist("Sheep", "ArtFavor", "Public Domain");
	about->addArtist("Star", "Tom Webb", "Public Domain");
	about->addArtist("Strawberry", wxEmptyString, "CC0/Public Domain"); // http://www.clker.com/clipart-25176.html (originally hosten on OCAL)
	about->addArtist("Tomato", "Chrisdesign", "Public Domain");
	about->addArtist("Top", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98187
	about->addArtist("Train", "Aitor Avila", "Public Domain");
	about->addArtist("Trumpet", "Gerald_G", "Public Domain");
	about->addArtist("Turtle", "valessiobrito", "Public Domain");
	about->addArtist("Udon", "Jordan Irwin (derivative)", "Public Domain");
	about->addArtist("Ukulele", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98182
	about->addArtist("Umbrella", "LX", "Public Domain");
	about->addArtist("Urchin", "Jordan Irwin", "Public Domain");
	about->addArtist("Unicycle", "AndrewDressel/Underpants", "Attribution-ShareAlike 3.0"); // https://commons.wikimedia.org/wiki/File:Unicycle.svg
	about->addArtist("Vase", "J_Alves", "Public Domain");
	about->addArtist("Vegetables", "johnny_automatic", "Public Domain");
	about->addArtist("Violin", "papapishu", "Public Domain");
	about->addArtist("Volleyball", "Andrea Bianchini", "Public Domain");
	about->addArtist("Vulture", "ArtFavor", "Public Domain");
	about->addArtist("Wagon", "Greg", "Public Domain");
	about->addArtist("Watch", "webmichl", "Public Domain");
	about->addArtist("Watermelon", "Gerald_G", "Public Domain");
	about->addArtist("Whale", "ArtFavor", "Public Domain");
	about->addArtist("Whistle", "kelan", "Public Domain");
	about->addArtist("X Cookie", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98172
	// artist: Jordi March i Nogué
	about->addArtist("X-Ray", _T("Jordi March i Nogu\u00E9"), "CC BY-SA 3.0"); // https://commons.wikimedia.org/wiki/File:X-rays_chest_icon.svg
	about->addArtist("Xiphias", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98166
	about->addArtist("Xun", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98276
	about->addArtist("Xylophone", "Gerald_G", "Public Domain");
	about->addArtist("Yak", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98167
	about->addArtist("Yam", "johnny_automatic", "Public Domain");
	about->addArtist("Yangqin", "koika/Jordan Irwin", "Attribution-ShareAlike 3.0"); // https://opengameart.org/node/79702
	about->addArtist("Yin Yang", "Stellaris", "Public Domain");
	about->addArtist("Yo-Yo", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98186
	about->addArtist("Zebra", "johnny_automatic", "Public Domain");
	about->addArtist("Zipline", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98185
	about->addArtist("Zipper", wxEmptyString, "Public Domain");
	about->addArtist("Zucchini", "Jordan Irwin (derivative)", "Public Domain"); // https://opengameart.org/node/98173
	about->addArtist("Zurna", "Jordan Irwin", "Public Domain"); // https://opengameart.org/node/98183

	about->addComposer("Accordion", wxEmptyString, "Public Domain");
	about->addComposer("Airplane", wxEmptyString, "Public Domain");
	about->addComposer("Bagpipes", "kyles", "CC0/Public Domain"); // https://freesound.org/people/kyles/sounds/453749/
	about->addComposer("Bird", "Jc Guan", "Public Domain"); // http://soundbible.com/340-Bird-Song.html
	about->addComposer("Bicycle", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16320
	about->addComposer("Cat", wxEmptyString, "Public Domain");
	about->addComposer("Clarinet", wxEmptyString, "Public Domain");
	about->addComposer("Clock", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16323
	about->addComposer("Dog", wxEmptyString, "Public Domain");
	about->addComposer("Dolphin");
	about->addComposer("Drums", "imakepitart", "Public Domain"); // https://commons.wikimedia.org/wiki/File:Drums.ogg (originally from pdsounds.org)
	about->addComposer("Electric Guitar", "Mattgirling", "Attribution-ShareAlike 3.0"); // https://commons.wikimedia.org/wiki/File:GuitarRig4.ogg
	about->addComposer("Elephant", wxEmptyString, "Public Domain");
	about->addComposer("Fire", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16327
	about->addComposer("Flute", wxEmptyString, "Public Domain");
	about->addComposer("Frog", wxEmptyString, "Public Domain");
	about->addComposer("Grasshopper", "straget", "CC0/Public Domain"); // https://freesound.org/people/straget/sounds/401939/
	about->addComposer("Guitar");
	about->addComposer("Harp", wxEmptyString, "Public Domain");
	about->addComposer("Horn(sound", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16322
	about->addComposer("Horse", wxEmptyString, "Public Domain");
	about->addComposer("Jaguar", wxEmptyString, "Public Domain");
	about->addComposer("Jug", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16325
	about->addComposer("Kazoo", "NoiseCollector", "Sampling Plus 1.0"); // incorrect license or relicensed under CC BY: https://freesound.org/people/NoiseCollector/sounds/7781/
	about->addComposer("Keyboard", "Trollarch2", "CC0/Public Domain"); // https://freesound.org/people/Trollarch2/sounds/331656/
	about->addComposer("Lion", wxEmptyString, "Public Domain");
	about->addComposer("Maracas", wxEmptyString, "Public Domain");
	about->addComposer("Mouse");
	about->addComposer("Oboe", "acclivity", "Sampling Plus 1.0"); // incorrect license or relicensed under CC BY-NC: https://freesound.org/people/acclivity/sounds/22686/
	about->addComposer("Owl", wxEmptyString, "Public Domain");
	about->addComposer("Piano", wxEmptyString, "Public Domain");
	about->addComposer("Pig", "Robinhood76", "Sampling Plus 1.0"); // incorrect license or relicensed under CC BY-NC: https://freesound.org/people/Robinhood76/sounds/76796/
	about->addComposer("Quail");
	about->addComposer("Quarter", "greenvwbeetle", "CC0/Public Domain"); // https://freesound.org/people/greenvwbeetle/sounds/423332/
	about->addComposer("Quena", "CarlosCarty", "CC BY"); // https://freesound.org/people/CarlosCarty/sounds/463711/
	about->addComposer("Raccoon");
	about->addComposer("Recorder", "Cailyn");
	about->addComposer("Rooster", "poorenglishjuggler", "CC0/Public Domain"); // https://freesound.org/people/poorenglishjuggler/sounds/269496/
	about->addComposer("Saxophone", "Sylenius", "Public Domain"); // https://en.wikipedia.org/wiki/File:Reed_phase.ogg
	about->addComposer("Sheep", "mikewest", "CC0/Public Domain"); // https://freesound.org/people/mikewest/sounds/414342/
	about->addComposer("Train", wxEmptyString, "Public Domain");
	about->addComposer("Trumpet ", "sorohanro", "Sampling Plus 1.0"); // incorrect license or relicensed under CC BY: https://freesound.org/people/sorohanro/sounds/77706/
	about->addComposer("Ukulele", "Henry Kailimai", "Attribution-ShareAlike 3.0"); // https://commons.wikimedia.org/wiki/File:Ukelele_-_Kailimai%27s_hene_-_Ukepedia.ogg
	about->addComposer("Umbrella", "j1987", "CC0/Public Domain"); // https://freesound.org/people/j1987/sounds/73028/
	about->addComposer("Violin", _T("Edit Kov\u00E1cs"), "Attribution-ShareAlike 2.0 Germany"); // https://commons.wikimedia.org/wiki/File:Whiskey-youre-the-devil-violin.ogg
	about->addComposer("Vulture", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16329
	about->addComposer("Watch", "Nicolas4677", "CC0/Public Domain"); // https://freesound.org/people/Nicolas4677/sounds/446611/
	about->addComposer("Whale", wxEmptyString, "Public Domain"); // https://www.freesoundeffects.com/free-track/whale2-466223/ (claims license is "non-commercial", need to check other sources)
	about->addComposer("Whistle", "pawsound", "CC0/Public Domain"); // https://freesound.org/people/pawsound/sounds/154873/
	about->addComposer("Xun" "franeknflute", "CC BY"); // https://freesound.org/people/franeknflute/sounds/200419/
	about->addComposer("Xylophone", "Tristan", "Sampling Plus 1.0"); // incorrect license of relicensed under CC0: https://freesound.org/people/Tristan/sounds/19459/
	about->addComposer("Zebra"); // https://www.partnersinrhyme.com/soundfx/safari_sounds/safari_zebra2_wav.shtml (unsure of license)
	about->addComposer("Zipper", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16332
	about->addComposer("Zurna", "cdrk", "Sampling Plus 1.0"); // incorrect license or relicensed under CC BY: https://freesound.org/people/cdrk/sounds/18661/

	about->initToolkitInfo();
	about->ShowModal();

	// CenterOnParent is called in the constructor
}

void MainWindow::OnFrameFocus(wxFocusEvent& event) {
	canvas->SetFocus();
}

void MainWindow::OnToggleLogWindow(wxCommandEvent& event) {
	toggleLogWindow();
}
