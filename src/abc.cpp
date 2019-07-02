#include "abc.h"
#include "category.h"
#include "env.h"
#include "gnrcabt.h"
#include "id.h"
#include "log.h"
#include "sound.h"

#include <wx/ffile.h>
#include <wx/regex.h>


// icon
#if !defined(WIN32) && !defined(WIN64)
#define ICON1 _T("myabcs.png")
#else
#define ID_ICON1
#define ICON1 wxICON(ID_ICON1)
#endif


static ResourceObject currentResource;
static ResourceObject winResource;


// checks if a string contains only alphabetic characters
static bool isAlpha(wxString s) {
	return wxRegEx(_T("^[A-Za-z]+$")).Matches(s);
}


MainWindow::MainWindow() :
		wxFrame(NULL, ID_WINDOW, wxEmptyString, wxDefaultPosition, wxSize(400, 550),
				wxDEFAULT_FRAME_STYLE &~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) {
	resourceList = ResourceList();
	winResource = ResourceObject(_T("ribbon"), wxEmptyString);

	// Get the executable's filename
	wxString argv0 = wxString(executable);

	// FIXME: Currently uses current working directory to locate data
	//        rather than looking in the installed directory.
#ifdef ABCDIR
	installdir = _T(ABCDIR);
	installdir.Append(_T("/"));
#else
	installdir = dir_root;
#endif

	SetIcon(wxIcon(ICON1));

	// Tool Bar
	wxBitmap ico_exit(wxImage(_T("pic/exit.png")));
	wxBitmap ico_abc(wxImage(_T("pic/abc.png")));
	wxBitmap ico_anim(wxImage(_T("pic/animals.png")));
	wxBitmap ico_food(wxImage(_T("pic/food.png")));
	wxBitmap ico_inst(wxImage(_T("pic/instrument.png")));
	wxBitmap ico_toys(wxImage(_T("pic/toys.png")));

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
	menu->AddTool(ID_HELP, _T("Help"), wxBitmap(wxImage(_T("pic/help.png"))), wxNullBitmap, wxITEM_NORMAL,
			wxEmptyString, _T("Help"));
	menu->AddTool(wxID_ABOUT, _T("About"), wxBitmap(wxImage(_T("pic/info.png"))), wxNullBitmap, wxITEM_NORMAL,
			wxEmptyString, _T("About MyABCs"));
#ifdef DEBUG
	menu->AddTool(ID_LOG, _T("Log"), wxBitmap(wxImage(_T("pic/settings.png"))), wxNullBitmap, wxITEM_NORMAL,
			wxEmptyString, _T("Show/Hide log window"));
#endif
	menu->AddSeparator();
	menu->AddTool(wxID_EXIT, _T("Exit"), ico_exit, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, _T("Quit"));

	menu->SetBackgroundColour(_T("#84aee6"));
	menu->Realize();

	Connect(wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainWindow::OnSetCategory), 0, this);
	Connect(ID_HELP, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainWindow::OnHelp), 0, this);
	Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainWindow::OnAbout), 0, this);
#ifdef DEBUG
	Connect(ID_LOG, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainWindow::OnToggleLogWindow), 0, this);
#endif

	// Status bar
	status = new wxStatusBar(this, -1);
	SetStatusBar(status);

	// Background panel
	canvas = new wxPanel(this, ID_BG, wxDefaultPosition, wxDefaultSize, 0 | wxWANTS_CHARS);
	canvas->SetFocus();

	canvas->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(MainWindow::OnKeyDown), NULL, this);
	canvas->Connect(wxEVT_KEY_UP, wxKeyEventHandler(MainWindow::OnKeyUp), NULL, this);

	image = new wxStaticBitmap(canvas, -1, wxNullBitmap);
	letter = new wxStaticText(canvas, -1, _T(""), wxDefaultPosition, wxDefaultSize,
			wxALIGN_CENTRE);
	label = new wxStaticText(canvas, -1, _T(""));
	// FIXME: fonts
	letter->SetFont(wxFont(45, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	label->SetFont(wxFont(20, wxDEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

	// Layout
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->AddSpacer(5);
	sizer->Add(image, 1, wxALIGN_CENTER);
	sizer->Add(letter, 0, wxALIGN_CENTER);
	sizer->Add(label, 0, wxALIGN_CENTER);
	sizer->AddSpacer(10);

	canvas->SetAutoLayout(true);
	canvas->SetSizer(sizer);
	canvas->Layout();

	// Redirect focus to main panel
	Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(MainWindow::OnFrameFocus), 0, this);

	Center(); // Center the window on the screen
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

void MainWindow::LoadCategory(wxString cat_name) {
	// DEBUG:
	logMessage(wxString("Loading category: ").Append(cat_name));

	if (resourceList.count() != 0) {
		resourceList.clear();
	}

	cur_category = cat_name;
	resourceList.set(createCategory(cur_category));

	if (cur_category == "animal") {
		SetTitle(_T("Press a Key to See an Animal"));
		currentResource = resourceList.getObject("t");
		letter->SetLabel(_T("Animals"));
	} else if (cur_category == "music") {
		SetTitle(_T("Press a Key to See an Instrument"));
		currentResource = resourceList.getObject("g");
		letter->SetLabel(_T("Music"));
	} else if (cur_category == "food") {
		SetTitle(_T("Press a Key to See a Food"));
		currentResource = resourceList.getObject("h");
		letter->SetLabel(_T("Food"));
	} else if (cur_category == "toy") {
		SetTitle(_T("Press a Key to See a Toy"));
		currentResource = resourceList.getObject("w");
		letter->SetLabel(_T("Toys"));
	} else {
		currentResource = resourceList.getObject("a");
		SetTitle(_T("Find the letter on the keyboard"));
		ReloadDisplay(true);

		return;
	}

	label->SetLabel(wxEmptyString);
	ReloadDisplay(false);
	game_end = false;
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

void MainWindow::PlayAlphaSound() {
	soundPlayer->play(wxString::Format("sound/alpha/%s.wav", getCurrentLetter()));
}

void MainWindow::OnSetCategory(wxCommandEvent& event) {
	int id = event.GetId();

	if (id == wxID_EXIT) {
		Close(true);
	} else {
		if (soundIsInitialized()) {
			soundPlayer->stop();
		}

		if (id == ID_ANIMALS) {
			LoadCategory(_T("animal"));
		} else if (id == ID_MUSIC) {
			LoadCategory(_T("music"));
		} else if (id == ID_FOOD) {
			LoadCategory(_T("food"));
		} else if (id == ID_TOYS) {
			LoadCategory(_T("toy"));
		} else {
			LoadCategory(_T("main"));
		}
	}
}

// FIXME: if sound fails to play, space key release isn't caught (still valid?)
void MainWindow::OnKeyDown(wxKeyEvent& event) {
	// DEBUG:
	logMessage(wxString::Format("%s: Key down ...", cur_category));

	const int key_code = event.GetKeyCode();

	// TODO: failsafe check for wxKeyEvent
	wxChar pressed_key = char(key_code);
	wxString current_letter = getCurrentLetter();

	if (key_code == WXK_SPACE) {
		currentResource.playSound();
	} else if (key_code == WXK_TAB) {
		OnTab();
	} else if (cur_category != "main" && isAlpha(pressed_key)) {
		// DEBUG:
		logMessage(wxString::Format("Setting letter to: %c", pressed_key));

		if (pressed_key != current_letter) SetLetter(pressed_key);
		PlayAlphaSound();
	} else { // "main" category
		if (!game_end) {
			if (isAlpha(pressed_key)) {
				if (pressed_key == current_letter) {
					// FIXME: don't update display until after sound is done playing
					PlayAlphaSound();
					if (pressed_key == 'Z') {
						// DEBUG:
						logMessage("End of game");

						game_end = true;

						currentResource = winResource;
						letter->SetLabel(_T("CONGRATS!"));
						label->SetLabel(_T("Press \"ENTER\" to Play Again"));
						ReloadDisplay(false);
					} else {
						IncrementLetter(current_letter);
					}
				} else {
					// DEBUG:
					logMessage("Wrong key pressed");
				}
			} else if (key_code == WXK_BACK && current_letter != "A") {
				// DEBUG:
				logMessage("Backspacking");

				IncrementLetter(current_letter, false);
			} else {
				// DEBUG:
				logMessage(wxString::Format("Key is not alphabetic: %c", pressed_key));
			}
		} else { // game ended
			if (key_code == WXK_RETURN || key_code == WXK_NUMPAD_ENTER) {
				// DEBUG:
				logMessage("Restarting game");

				SetLetter("a");
				game_end = false;
			} else if (key_code == WXK_BACK) {
				// backspace after game end return to letter "Z"
				SetLetter("Z");
				game_end = false;
			} else {
				// DEBUG:
				logMessage("Game has ended, press \"Enter\" or change category");
			}
		}
	}

	event.Skip();
}

void MainWindow::OnKeyUp(wxKeyEvent& event) {
	logMessage(_T("Key up ..."));

	event.Skip();
}

void MainWindow::OnTab() {
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

// Help and About dialogs

void MainWindow::OnHelp(wxMenuEvent& event) {
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

void MainWindow::OnAbout(wxMenuEvent& event) {
	// About Dialog
	GenericAbout *about = new GenericAbout(this, -1);
	about->SetIcon(wxIcon(ICON1));
	about->SetImage(_T("myabcs.png"));
	about->SetName(_T("MyABCs"));
	about->SetVersion(_T("0.4.5"));
	about->SetCopyright(_T("\u00A9 Jordan Irwin 2010-2019"));
	about->SetAbout(_T("MyABCs is educational software for young children to learn\nthe English alphabet and get familiar with a keyboard"));

	about->AddCredit(_T("Jordan Irwin"), CREDIT_DEVELOPER);
	about->AddCredit(_T("Jordan Irwin"), CREDIT_PACKAGER);

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

	about->SetChangelog(cl_text);

	about->AddArtist(_T("ABC Blocks"), _T("Petri Lummemaki "), _T("Public Domain"));
	about->AddArtist(_T("Accordion"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Accordion(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Airplane"), _T("Jarno Vasamaa"), _T("Public Domain"));
	about->AddArtist(_T("Airplane(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Anklet"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Angelfish"), _T("Jonathon Love"), _T("Public Domain"));
	about->AddArtist(_T("Apple"), _T("Chrisdesign"), _T("Public Domain"));
	about->AddArtist(_T("Bagpipes"), _T("Jordan Irwin (derivative)"), _T("Public Domain"));
	about->AddArtist(_T("Balloons"), _T("AJ"), _T("Public Domain"));
	about->AddArtist(_T("Bananas"), _T("nicubunu"), _T("Public Domain"));
	about->AddArtist(_T("Bird"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Bird(sound)"), _T("Jc Guan"), _T("Public Domain"));
	about->AddArtist(_T("Bicycle"), _T("alvolk"), _T("Public Domain"));
	about->AddArtist(_T("Cat"), _T("Francesco Rollandin"), _T("Public Domain"));
	about->AddArtist(_T("Cat(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Chalkboard"), _T("J_Alves"), _T("Public Domain"));
	about->AddArtist(_T("Cherries"), _T("Rocket000"), _T("Public Domain"));
	about->AddArtist(_T("Clarinet"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Clarinet(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Clock"), _T("Jonathan Dietrich"), _T("Public Domain"));
	about->AddArtist(_T("Dog"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Dog(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Doll"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Dolphin"), _T("Andrew Fitzsimon"), _T("Public Domain"));
	about->AddArtist(_T("Dolphin(sound)"));
	about->AddArtist(_T("Doughnut"), _T("worms_x"), _T("Public Domain"));
	about->AddArtist(_T("Drums"), _T("TheresaKnott"), _T("Public Domain"));
	about->AddArtist(_T("Drums(sound)"), _T("imakepitart"), _T("Public Domain"));
	about->AddArtist(_T("Earth"), _T("Dan Gerhrads"), _T("Public Domain"));
	about->AddArtist(_T("Easel"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Egg"), _T("dStulle"), _T("Public Domain"));
	about->AddArtist(_T("Electric Guitar"), _T("Chrisdesign"), _T("Public Domain"));
	about->AddArtist(_T("Electric Guitar(sound)"), _T("Mattgirling"), _T("Attribution-ShareAlike 3.0"));
	about->AddArtist(_T("Elephant"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Elephant(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Fire"), _T("Valessio Soares de Brito"), _T("Public Domain"));
	about->AddArtist(_T("Flute"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Flute(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Football"), _T("molumen"), _T("Public Domain"));
	about->AddArtist(_T("Fries"), _T("Juliane Krug"), _T("Public Domain"));
	about->AddArtist(_T("Frog"), _T("leland_mcinnes"), _T("Public Domain"));
	about->AddArtist(_T("Frog(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Giraffe"), _T("Packard Jennings"), _T("Public Domain"));
	about->AddArtist(_T("Glove"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Grapes"), _T("Jean-Victor Balin"), _T("Public Domain"));
	about->AddArtist(_T("Grasshopper"), _T("Francesco Rollandin"), _T("Public Domain"));
	about->AddArtist(_T("Guitar"), _T("papapishu"), _T("Public Domain"));
	about->AddArtist(_T("Harp"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Harp(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Hat"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Horn"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Horse"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Horse(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Hot Dog"), _T("Juliane Krug"), _T("Public Domain"));
	about->AddArtist(_T("Ice Cream"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Ice Skate"), _T("Francesco Rollandin"), _T("Public Domain"));
	about->AddArtist(_T("Igloo"), _T("Jose Hevia"), _T("Public Domain"));
	about->AddArtist(_T("Iguana"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Ipu"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Jaguar"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Jaguar(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Jar"), _T("DTRave"), _T("Public Domain"));
	about->AddArtist(_T("Jug"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Juice"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Jump Rope"), _T("johnny_automatic"), _T("Public Domain"));
	about->AddArtist(_T("Kazoo"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Kazoo(sound)"), _T("NoiseCollector"), _T("Sampling Plus 1.0"));
	about->AddArtist(_T("Ketchup"), _T("Alexandre Norman"), _T("Public Domain"));
	about->AddArtist(_T("Keyboard"), _T("yeKcim"), _T("Public Domain"));
	about->AddArtist(_T("Kite"), _T("schoolfreeware"), _T("Public Domain"));
	about->AddArtist(_T("Koala"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Ladybug"), _T("lalolalo"), _T("Public Domain"));
	about->AddArtist(_T("Lasagna"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Lion"), _T("Francesco Rollandin"), _T("Public Domain"));
	about->AddArtist(_T("Lion(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Lunchbox"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Lute"), _T("papapishu"), _T("Public Domain"));
	about->AddArtist(_T("Magnet"), _T("Francesco Rollandin"), _T("Public Domain"));
	about->AddArtist(_T("Magnifying Glass"), _T("TheStructorr"), _T("Public Domain"));
	about->AddArtist(_T("Maracas"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Maracas(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Mouse"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Mouse(sound)"));
	about->AddArtist(_T("Mushroom"), _T("Tanguy JACQ"), _T("Public Domain"));
	about->AddArtist(_T("Necklace"), _T("wsnaccad"), _T("Public Domain"));
	about->AddArtist(_T("Newspaper"), _T("Aubanel Monnier"), _T("Public Domain"));
	about->AddArtist(_T("Ney"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Newt"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Noodle"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Oboe"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Oboe(sound)"), _T("acclivity"), _T("Sampling Plus 1.0"));
	about->AddArtist(_T("Olive"), _T("johnny_automatic"), _T("Public Domain"));
	about->AddArtist(_T("Onion"), _T("Chrisdesign"), _T("Public Domain"));
	about->AddArtist(_T("Orca"), _T("Matthew Gates"), _T("Public Domain"));
	about->AddArtist(_T("Origami"), _T("badaman"), _T("Public Domain"));
	about->AddArtist(_T("Owl"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Owl(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Paint"), _T("valessiobrito"), _T("Public Domain"));
	about->AddArtist(_T("Paper Airplane"), _T("nicubunu"), _T("Public Domain"));
	about->AddArtist(_T("Piano"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Piano(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Pretzel"), _T("Nathan Eady"), _T("Public Domain"));
	about->AddArtist(_T("Pig"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Pig(sound)"), _T("Robinhood76"), _T("Sampling Plus 1.0"));
	about->AddArtist(_T("Quail"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Quail(sound)"));
	about->AddArtist(_T("Quarter"), _T("Paul Sherman"), _T("Public Domain"));
	about->AddArtist(_T("Quena"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Quiche"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Quilt"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Raccoon"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Radishes"), _T("Francesco Rollandin"), _T("Public Domain"));
	about->AddArtist(_T("Recorder"), _T("zeimusu"), _T("Public Domain"));
	about->AddArtist(_T("Recorder(sound)"), _T("Cailyn"));
	about->AddArtist(_T("Rocking Horse"), _T("Chrisdesign"), _T("Public Domain"));
	about->AddArtist(_T("Rooster"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Rooster(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Saxophone"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Sax(sound)"), _T("Sylenius"), _T("Public Domain"));
	about->AddArtist(_T("Seesaw"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Sheep"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Sheep(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Star"), _T("Tom Webb"), _T("Public Domain"));
	about->AddArtist(_T("Strawberry"), _T("baroquon"), _T("Public Domain"));
	about->AddArtist(_T("Tomato"), _T("Chrisdesign"), _T("Public Domain"));
	about->AddArtist(_T("Top"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Train"), _T("Aitor Avila"), _T("Public Domain"));
	about->AddArtist(_T("Train(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Trumpet"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Trumpet (sound)"), _T("sorohanro"), _T("Sampling Plus 1.0"));
	about->AddArtist(_T("Turtle"), _T("valessiobrito"), _T("Public Domain"));
	about->AddArtist(_T("Udon"), _T("Jordan Irwin (derivative)"), _T("Public Domain"));
	about->AddArtist(_T("Ukulele"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Ukulele(sound)"), _T("Henry Kailimai"), _T("Attribution-ShareAlike 3.0"));
	about->AddArtist(_T("Umbrella"), _T("LX"), _T("Public Domain"));
	about->AddArtist(_T("Urchin"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Unicycle"), _T("AndrewDressel/Underpants"), _T("Attribution-ShareAlike 3.0"));
	about->AddArtist(_T("Vase"), _T("J_Alves"), _T("Public Domain"));
	about->AddArtist(_T("Vegetables"), _T("johnny_automatic"), _T("Public Domain"));
	about->AddArtist(_T("Violin"), _T("papapishu"), _T("Public Domain"));
	about->AddArtist(_T("Violin(sound)"), _T("Edit Kov\u00E1cs"), _T("Attribution-ShareAlike 2.0 Germany"));
	about->AddArtist(_T("Volleyball"), _T("Andrea Bianchini"), _T("Public Domain"));
	about->AddArtist(_T("Vulture"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Vulture(sound)"), _T("Jordan Irwin"), _T("CC0/Public Domain"));
	about->AddArtist(_T("Wagon"), _T("Greg"), _T("Public Domain"));
	about->AddArtist(_T("Watch"), _T("webmichl"), _T("Public Domain"));
	about->AddArtist(_T("Watermelon"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Whale"), _T("ArtFavor"), _T("Public Domain"));
	about->AddArtist(_T("Whale(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Whistle"), _T("kelan"), _T("Public Domain"));
	about->AddArtist(_T("Whistle(sound)"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("X Cookie"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("X-Ray (Skeleton)"), _T("johnny_automatic"), _T("Public Domain"));
	about->AddArtist(_T("Xiphias"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Xun"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Xylophone"), _T("Gerald_G"), _T("Public Domain"));
	about->AddArtist(_T("Xylophone(sound)"), _T("Tristan"), _T("Sampling Plus 1.0"));
	about->AddArtist(_T("Yak"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Yam"), _T("johnny_automatic"), _T("Public Domain"));
	about->AddArtist(_T("Yangqin"), _T("koika/Jordan Irwin"), _T("Attribution-ShareAlike 3.0"));
	about->AddArtist(_T("Yin Yang"), _T("Stellaris"), _T("Public Domain"));
	about->AddArtist(_T("Yo-Yo"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Zebra"), _T("johnny_automatic"), _T("Public Domain"));
	about->AddArtist(_T("Zebra(sound)"));
	about->AddArtist(_T("Zipline"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Zipper"), wxEmptyString, _T("Public Domain"));
	about->AddArtist(_T("Zipper(sound)"), _T("tmkappelt"), _T("Sampling Plus 1.0"));
	about->AddArtist(_T("Zucchini"), _T("Jordan Irwin (derivative)"), _T("Public Domain"));
	about->AddArtist(_T("Zurna"), _T("Jordan Irwin"), _T("Public Domain"));
	about->AddArtist(_T("Zurna(sound)"), _T("cdrk"), _T("Sampling Plus 1.0"));

	about->ShowModal();

	// CenterOnParent is called in the constructor
}

void MainWindow::OnFrameFocus(wxFocusEvent& event) {
	canvas->SetFocus();
}

void MainWindow::OnToggleLogWindow(wxMenuEvent& event) {
	toggleLogWindow();
}
