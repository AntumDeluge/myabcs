#include "abc.h"
#include "sound.h"

#include <stddef.h>

using namespace std;

const int ID_WINDOW = wxNewId();
const int ID_BG = wxNewId();

wxString MainWindow::cur_sound(wxEmptyString);

MainWindow::MainWindow(const wxString& title)
: wxFrame(NULL, ID_WINDOW, title, wxDefaultPosition, wxSize(400, 550),
wxDEFAULT_FRAME_STYLE &~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) {
    // Get the executable's directory
    exedir = new wxString(wxStandardPaths::Get().GetExecutablePath());
    wxFileName filename(*exedir);
    // Get the executable's filename
    wxString argv0 = filename.GetFullName();
    // Subtract the filename from the the full pathname
    unsigned int x;
    for (x = 0; x < argv0.Length(); x++) {
        exedir->RemoveLast();
    }

    // FIXME: Currently uses current working directory to locate data
    //        rather than looking in the installed directory.
#ifdef ABCDIR
    installdir = _T(ABCDIR);
    installdir.Append(_T("/"));
#else
    installdir = exedir;
#endif

    // Add support for images
    wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxICOHandler);
	wxImage::AddHandler(new wxXPMHandler);

    SetIcon(wxIcon(ICON1));

    // Set these two variables to show that play is ready
    gameend = false;
    isplaying = false;
    canspace = true;
    cantab = true;
    cankey = true;

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
    menu->AddSeparator();
    menu->AddTool(wxID_EXIT, _T("Exit"), ico_exit, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, _T("Quit"));

    menu->SetBackgroundColour(_T("#84aee6"));
    menu->Realize();

    Connect(wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainWindow::SetMode), 0, this);
    Connect(ID_HELP, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainWindow::OnHelp), 0, this);
    Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(MainWindow::OnAbout), 0, this);

    // Status bar
    status = new wxStatusBar(this, -1);
    SetStatusBar(status);

    // Background panel
    bg = new wxPanel(this, ID_BG, wxDefaultPosition, wxDefaultSize, 0 | wxWANTS_CHARS);
    bg->SetFocus();

    image = new ImageDisplay(bg, -1, wxNullBitmap);
    letter = new wxStaticText(bg, -1, _T(""), wxDefaultPosition, wxDefaultSize,
            wxALIGN_CENTRE);
    label = new wxStaticText(bg, -1, _T(""));
    // FIXME: fonts
    letter->SetFont(wxFont(45, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    label->SetFont(wxFont(20, wxDEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    bg->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(MainWindow::OnKey), NULL, this);
    bg->Connect(wxEVT_KEY_UP, wxKeyEventHandler(MainWindow::OnKeyUp), NULL, this);

    // Catch events passed by threads
    //Connect(wxID_ANY, ID_TAB, wxCommandEventHandler(MainWindow::OnTab), 0, this);
    Connect(wxID_ANY, ID_KEY, wxCommandEventHandler(MainWindow::ChangeLetter), 0, this);
    Connect(wxID_ANY, ID_OTHER, wxCommandEventHandler(MainWindow::EnableAll), 0, this);

    // Initialize static current sound variable
    cur_sound = wxEmptyString;

    // Labels
    wxString alpha1 = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    wxString alpha2[] = {
        _T("AIRPLANE"), _T("BICYCLE"), _T("CLOCK"), _T("DOLPHIN"), _T("EARTH"), _T("FIRE"),
        _T("GRASSHOPPER"), _T("HAT"), _T("IGLOO"), _T("JAR"), _T("KEYBOARD"),
        _T("LADYBUG"), _T("MAGNIFYING GLASS"), _T("NEWSPAPER"), _T("ONION"),
        _T("PAPER AIRPLANE"), _T("QUARTER"), _T("RACCOON"), _T("STAR"),
        _T("TRAIN"), _T("UMBRELLA"), _T("VASE"), _T("WATCH"), _T("X-RAY"),
        _T("YIN YANG"), _T("ZIPPER")
    };
    wxString alpha3[] = {
        AIRPLANE, BICYCLE, CLOCK, DOLPHIN, EARTH, FIRE,
        GRASSHOPPER, HAT, IGLOO, JAR, KEYBOARD,
        LADYBUG, MAG_GLASS, NEWS, ONION,
        PAPER_PLANE, QUARTER, RACCOON, STAR,
        TRAIN, UMBRELLA, VASE, WATCH, X_RAY,
        YINYANG, ZIPPER
    };

    for (int a = 0; a < 26; a += 1) {
        letters[a] = alpha1[a];
        labels[a] = alpha2[a];
        images[a] = alpha3[a];
    }

    // Foods
    wxString food_pic[26] = {
        APPLE, BANANAS, CHERRIES, DOUGHNUT, EGG,
        FRIES, GRAPES, HOT_DOG, ICE_CREAM, JUICE,
        KETCHUP, LASAGNA, MUSHROOM, NOODLE, OLIVE,
        PRETZEL, QUICHE, RADISHES, STRAWBERRY, TOMATO,
        UDON, VEGETABLES, WATERMELON, XCOOKIE, YAM,
        ZUCCHINI
    };

    wxString food_name[26] = {
        _T("APPLE"), _T("BANANAS"), _T("CHERRIES"), _T("DOUGHNUT"), _T("EGG"),
        _T("FRIES"), _T("GRAPES"), _T("HOT DOG"), _T("ICE CREAM"), _T("JUICE"),
        _T("KETCHUP"), _T("LASAGNA"), _T("MUSHROOM"), _T("NOODLE"), _T("OLIVE"),
        _T("PRETZEL"), _T("QUICHE"), _T("RADISHES"), _T("STRAWBERRY"), _T("TOMATO"),
        _T("UDON"), _T("VEGETABLES"), _T("WATERMELON"), _T("X-COOKIE"), _T("YAM"),
        _T("ZUCCHINI")
    };

    for (int a = 0; a < 26; a += 1) {
        foods[0][a] = food_pic[a];
    }
    for (int a = 0; a < 26; a += 1) {
        foods[1][a] = food_name[a];
    }

    // Animals
    wxString ani_pic[26] = {
        ANGELFISH, BIRD, CAT, DOG, ELEPHANT, FROG,
        GIRAFFE, HORSE, IGUANA, JAGUAR, KOALA,
        LION, MOUSE, NEWT, OWL, PIG,
        QUAIL, ROOSTER, SHEEP, TURTLE, URCHIN,
        VULTURE, WHALE, XIPHIAS, YAK, ZEBRA
    };
    wxString ani_name[26] = {
        _T("ANGELFISH"), _T("BIRD"), _T("CAT"), _T("DOG"), _T("ELEPHANT"), _T("FROG"),
        _T("GIRAFFE"), _T("HORSE"), _T("IGUANA"), _T("JAGUAR"), _T("KOALA"),
        _T("LION"), _T("MOUSE"), _T("NEWT"), _T("OWL"), _T("PIG"),
        _T("QUAIL"), _T("ROOSTER"), _T("SHEEP"), _T("TURTLE"), _T("URCHIN"),
        _T("VULTURE"), _T("WHALE"), _T("XIPHIAS"), _T("YAK"), _T("ZEBRA")
    };

    for (int a = 0; a < 26; a += 1) {
        animals[0][a] = ani_pic[a];
    }
    for (int a = 0; a < 26; a += 1) {
        animals[1][a] = ani_name[a];
    }

    // Music
    wxString music_pic[26] = {
        ACCORDION, BAGPIPES, CLARINET, DRUMS, EGUITAR,
        FLUTE, GUITAR, HARP, IPU, JUG,
        KAZOO, LUTE, MARACAS, NEY, OBOE, PIANO,
        QUENA, RECORDER, SAX, TRUMPET, UKULELE,
        VIOLIN, WHISTLE, XYLOPHONE, YANGQIN, ZURNA
    };
    wxString music_name[26] = {
        _T("ACCORDION"), _T("BAGPIPES"), _T("CLARINET"), _T("DRUMS"), _T("ELECTRIC GUITAR"),
        _T("FLUTE"), _T("GUITAR"), _T("HARP"), _T("IPU"), _T("JUG"),
        _T("KAZOO"), _T("LUTE"), _T("MARACAS"), _T("NEY"), _T("OBOE"), _T("PIANO"),
        _T("QUENA"), _T("RECORDER"), _T("SAXOPHONE"), _T("TRUMPET"), _T("UKULELE"),
        _T("VIOLIN"), _T("WHISTLE"), _T("XYLOPHONE"), _T("YANGQIN"), _T("ZURNA")
    };

    for (int a = 0; a < 26; a += 1) {
        musics[0][a] = music_pic[a];
    }
    for (int a = 0; a < 26; a += 1) {
        musics[1][a] = music_name[a];
    }

    // Toys
    wxString toys_pic[26] = {
        ANKLET, BALLOONS, CHALKBOARD, DOLL, EASEL,
        FOOTBALL, GLOVE, HORN, ICE_SKATE, JUMP_ROPE,
        KITE, LUNCHBOX, MAGNET, NECKLACE, ORIGAMI, PAINT,
        QUILT, ROCK_HORSE, SEESAW, TOP, UNICYCLE,
        VOLLEYBALL, WAGON, XYLOPHONE, YOYO, ZIPLINE
    };
    wxString toys_name[26] = {
        _("ANKLET"), _T("BALLOONS"), _T("CHALKBOARD"), _T("DOLL"), _T("EASEL"),
        _T("FOOTBALL"), _T("GLOVE"), _T("HORN"), _T("ICE SKATE"), _T("JUMP ROPE"),
        _T("KITE"), _T("LUNCHBOX"), _T("MAGNET"), _T("NECKLACE"), _T("ORIGAMI"), _T("PAINT"),
        _T("QUILT"), _T("ROCKING HORSE"), _T("SEESAW"), _T("TOP"), _T("UNICYCLE"),
        _T("VOLLEYBALL"), _T("WAGON"), _T("XYLOPHONE"), _T("YO-YO"), _T("ZIP-LINE")
    };

    for (int a = 0; a < 26; a += 1) {
        toys[0][a] = toys_pic[a];
    }
    for (int a = 0; a < 26; a += 1) {
        toys[1][a] = toys_name[a];
    }


    // wav sounds for letters
    wxString temp_sounds[2][26] = {
        {
            A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        },
        {
            _T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"),
            _T("J"), _T("K"), _T("L"), _T("M"), _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"),
            _T("S"), _T("T"), _T("U"), _T("V"), _T("W"), _T("X"), _T("Y"), _T("Z")
        }
    };
    for (int a = 0; a < 26; a += 1) {
        letter_sounds[0][a] = temp_sounds[0][a];
    }
    for (int a = 0; a < 26; a += 1) {
        letter_sounds[1][a] = temp_sounds[1][a];
    }

    // Layout
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddSpacer(5);
    sizer->Add(image, 1, wxALIGN_CENTER);
    sizer->Add(letter, 0, wxALIGN_CENTER);
    sizer->Add(label, 0, wxALIGN_CENTER);
    sizer->AddSpacer(10);

    bg->SetAutoLayout(true);
    bg->SetSizer(sizer);
    bg->Layout();

    // Start the app with ABC mode
    wxCommandEvent start(wxEVT_COMMAND_MENU_SELECTED, ID_ABC);
    SetMode(start);

    // Redirect focus to main panel
    Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(MainWindow::OnFrameFocus), 0, this);

    Center(); // Center the window on the screen
}

void MainWindow::SetMode(wxCommandEvent& event) {
    int id = event.GetId();

    if (id == wxID_EXIT) {
        Close(true);
    } else {
        gameend = false;
        if (soundIsInitialized()) {
        	soundPlayer->stop();
        }

        if (id == ID_ABC) {
            SetTitle(_T("Find the letter on the keyboard"));
            image->SetImageG(images[0]);
            letter->SetLabel(letters[0]);
            label->SetLabel(labels[0]);
        } else {
            if (id == ID_ANIMALS) {
                SetTitle(_T("Press a Key to See an Animal"));
                image->SetImageG(TURTLE);
                letter->SetLabel(_T("Animals"));
                label->SetLabel(_T(""));
            } else if (id == ID_MUSIC) {
                SetTitle(_T("Press a Key to See an Instrument"));
                image->SetImageG(GUITAR);
                letter->SetLabel(_T("Music"));
                label->SetLabel(_T(""));
            } else if (id == ID_FOOD) {
                SetTitle(_T("Press a Key to See a Food"));
                image->SetImageG(HOT_DOG);
                letter->SetLabel(_T("Food"));
                label->SetLabel(_T(""));
            } else if (id == ID_TOYS) {
                SetTitle(_T("Press a Key to See a Toy"));
                image->SetImageG(WAGON);
                letter->SetLabel(_T("Toys"));
                label->SetLabel(_T(""));
            }
        }
    }
    bg->SetFocusIgnoringChildren();
    bg->Refresh();
    bg->Layout();
}

void MainWindow::OnKey(wxKeyEvent& event) {
    if (!isplaying && cankey && canspace) {
        // Get key that was pressed
        int keycode = event.GetKeyCode();
        key = char(keycode);

        // Catch Tab and Space
        if (keycode == WXK_TAB) {
            if (cantab) {
                // Make sure that game is playable upon switching modes
                if (gameend) gameend = false;

                cantab = false;
                OnTab();
                //rc = pthread_create(&thread1, NULL, TabThread, this);
            }
        } else if (keycode == WXK_SPACE) {
            canspace = false;
            PlaySound();
        } else if (keycode == WXK_RETURN or keycode == WXK_NUMPAD_ENTER) {
            if (gameend) {
                gameend = false;
                soundPlayer->stop();
                image->SetImageG(images[0]);
                letter->SetLabel(letters[0]);
                label->SetLabel(labels[0]);

                bg->Layout();
                bg->Refresh();
            }
        } else {
            if (!gameend) {
                cankey = false;
                // Get currently displayed letter to compare with key pressed
                wxString getletter = letter->GetLabel();
                if (getletter == _T("Food") or getletter == _T("Animals")
                        or getletter == _T("Music") or getletter == _T("Toys")) {
                    // Choose a random letter to cancel "Food", "Animals" and "Music" as displayed letters
                    cur_letter = 'x';
                } else cur_letter = *getletter.char_str();

                // Set the sound to be played
                cur_sound = wxString::Format(_T("sound/alpha/%c.wav"), key);

                // For ABC mode make sure that key pressed is same as letter displayed
                if (menu->GetToolState(ID_ABC)) {
                    if (key == cur_letter) {
                        isplaying = true;
                        MainWindow::GoABC();
                    } else if (key == WXK_BACK) {
                        // Move back one letter
                        for (int x = 26; x > 0; x -= 1) {
                            if (cur_letter == letters[x]) {
                            	image->SetImageG(images[x-1]);
                                letter->SetLabel(letters[x - 1]);
                                label->SetLabel(labels[x - 1]);
                                bg->Refresh();
                                bg->Layout();
                            }
                        }
                    }
                } else {
                    MainWindow::GoOther();
                }
            } else {
                if (keycode == WXK_BACK) {
                    // Go back to letter "Z"
                    gameend = false;
                    soundPlayer->stop();
                    image->SetImageG(images[25]);
                    letter->SetLabel(letters[25]);
                    label->SetLabel(labels[25]);
                    bg->Refresh();
                    bg->Layout();
                }
            }
        }
    }
    event.Skip();
}

void MainWindow::OnTab()//wxCommandEvent& event)
{
    /* Changes game modes via the "Tab" key */
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
    //cantab = true;
}

void MainWindow::OnKeyUp(wxKeyEvent& event) {
    // Enables tabbing after the "tab" key is released to prevent cycling too quickly through modes
    int keycode = event.GetKeyCode();
    if (keycode == WXK_TAB) cantab = true;
    else if (keycode == WXK_SPACE) canspace = true;
    else cankey = true;
}

void MainWindow::GoABC() {
    // Start the thread to play a sound
    rc = pthread_create(&thread1, NULL, KeyThread, this);
}

void MainWindow::GoOther() {
    for (int a = 0; a < 26; a += 1) {
        if (letters[a] == key) {
            isplaying = true;

            // Don't refresh images if current letter is pressed
            if (key != cur_letter) {
                if (menu->GetToolState(ID_FOOD)) {
                	image->SetImageG(foods[0][a]);
                    letter->SetLabel(letters[a]);
                    label->SetLabel(foods[1][a]);
                } else if (menu->GetToolState(ID_ANIMALS)) {
                	image->SetImageG(animals[0][a]);
                    letter->SetLabel(letters[a]);
                    label->SetLabel(animals[1][a]);
                } else if (menu->GetToolState(ID_MUSIC)) {
                	image->SetImageG(musics[0][a]);
                    letter->SetLabel(letters[a]);
                    label->SetLabel(musics[1][a]);
                } else if (menu->GetToolState(ID_TOYS)) {
                	image->SetImageG(toys[0][a]);
                    letter->SetLabel(letters[a]);
                    label->SetLabel(toys[1][a]);
                }
                bg->Layout();
                bg->Refresh();
            }
            rc = pthread_create(&thread1, NULL, OtherThread, this);
        }
    }
}

void MainWindow::EnableAll(wxCommandEvent& event) {
    //canspace = true;
    cantab = true;
    isplaying = false;
}

void MainWindow::ChangeLetter(wxCommandEvent& event) {
    if (key == cur_letter) {
        if (cur_letter != 'Z') {
            for (int a = 0; a < 26; a += 1) {
                if (letters[a] == key) {
                	image->SetImageG(images[a+1]);
                    letter->SetLabel(letters[a + 1]);
                    label->SetLabel(labels[a + 1]);
                }
            }
        } else if (cur_letter == 'Z') {
            gameend = true;
            image->SetImageG(RIBBON);
            letter->SetLabel(_T("CONGRATS!"));
            label->SetLabel(_T("Press \"ENTER\" to Play Again"));
            cur_sound = CHEER;
            rc = pthread_create(&thread1, NULL, OtherThread, this);
        }
    }

    bg->Layout();
    bg->Refresh();
    isplaying = false; // Enable pressing keys
}

void MainWindow::PlaySound() {
    wxString name = label->GetLabel();
    wxString sounds[2][131] = {
        {
            _T("ACCORDION"), _T("AIRPLANE"), _T("ANGELFISH"), _T("ANKLET"), _T("APPLE"),
            _T("BAGPIPES"), _T("BALLOONS"), _T("BANANAS"), _T("BICYCLE"), _T("BIRD"),
            _T("CAT"), _T("CHALKBOARD"), _T("CHERRIES"), _T("CLARINET"), _T("CLOCK"),
            _T("DOG"), _T("DOLL"), _T("DOLPHIN"), _T("DOUGHNUT"), _T("DRUMS"),
            _T("EARTH"), _T("EASEL"), _T("EGG"), _T("ELECTRIC GUITAR"), _T("ELEPHANT"),
            _T("FIRE"), _T("FLUTE"), _T("FOOTBALL"), _T("FRIES"), _T("FROG"),
            _T("GIRAFFE"), _T("GLOVE"), _T("GRAPES"), _T("GRASSHOPPER"), _T("GUITAR"),
            _T("HARP"), _T("HORSE"), _T("HAT"), _T("HORN"), _T("HOT DOG"),
            _T("IGUANA"), _T("ICE CREAM"), _T("ICE SKATE"), _T("IGLOO"), _T("IPU"),
            _T("JAGUAR"), _T("JAR"), _T("JUG"), _T("JUICE"), _T("JUMP ROPE"),
            _T("KAZOO"), _T("KETCHUP"), _T("KEYBOARD"), _T("KITE"), _T("KOALA"),
            _T("LADYBUG"), _T("LASAGNA"), _T("LUNCHBOX"), _T("LION"), _T("LUTE"),
            _T("MAGNET"), _T("MAGNIFYING GLASS"), _T("MARACAS"), _T("MOUSE"), _T("MUSHROOM"),
            _T("NECKLACE"), _T("NEWSPAPER"), _T("NEWT"), _T("NEY"), _T("NOODLE"),
            _T("OBOE"), _T("OLIVE"), _T("ONION"), _T("ORIGAMI"), _T("OWL"),
            _T("PAINT"), _T("PAPER AIRPLANE"), _T("PIANO"), _T("PIG"), _T("PRETZEL"),
            _T("QUAIL"), _T("QUARTER"), _T("QUENA"), _T("QUICHE"), _T("QUILT"),
            _T("RACCOON"), _T("RADISHES"), _T("RECORDER"), _T("ROCKING HORSE"), _T("ROOSTER"),
            _T("SAXOPHONE"), _T("SHEEP"), _T("SEESAW"), _T("STAR"), _T("STRAWBERRY"),
            _T("TOMATO"), _T("TOP"), _T("TRAIN"), _T("TRUMPET"), _T("TURTLE"),
            _T("UDON"), _T("UKULELE"), _T("UMBRELLA"), _T("UNAGI"), _T("UNICYCLE"),
            _T("URCHIN"), _T("VASE"), _T("VEGETABLES"), _T("VIOLIN"), _T("VOLLEYBALL"),
            _T("VULTURE"), _T("WAGON"), _T("WATCH"), _T("WATERMELON"), _T("WHALE"),
            _T("WHISTLE"), _T("X-COOKIE"), _T("X-RAY"), _T("XIPHIAS"), _T("XYLOPHONE"),
            _T("YAK"), _T("YAM"), _T("YANGQIN"), _T("YIN YANG"), _T("YO-YO"),
            _T("ZEBRA"), _T("ZIP-LINE"), _T("ZIPPER"), _T("ZUCCHINI"), _T("ZURNA"),
            _T("XUN")
        },

        {
            _T("sound/accordion.wav"),
            _T("sound/airplane.wav"),
            _T("sound/angelfish.wav"),
            _T("sound/anklet.wav"),
            _T("sound/apple.wav"),
            _T("sound/bagpipes.wav"),
            _T("sound/balloons.wav"),
            _T("sound/bananas.wav"),
            _T("sound/bicycle.wav"),
            _T("sound/bird.wav"),
            _T("sound/cat.wav"),
            _T("sound/chalkboard.wav"),
            _T("sound/cherries.wav"),
            _T("sound/clarinet.wav"),
            _T("sound/clock.wav"),
            _T("sound/dog.wav"),
            _T("sound/doll.wav"),
            _T("sound/dolphin.wav"),
            _T("sound/doughnut.wav"),
            _T("sound/drums.wav"),
            _T("sound/earth.wav"),
            _T("sound/easel.wav"),
            _T("sound/egg.wav"),
            _T("sound/eguitar.wav"),
            _T("sound/elephant.wav"),
            _T("sound/fire.wav"),
            _T("sound/flute.wav"),
            _T("sound/football.wav"),
            _T("sound/fries.wav"),
            _T("sound/frog.wav"),
            _T("sound/giraffe.wav"),
            _T("sound/glove.wav"),
            _T("sound/grapes.wav"),
            _T("sound/grasshopper.wav"),
            _T("sound/guitar.wav"),
            _T("sound/harp.wav"),
            _T("sound/horse.wav"),
            _T("sound/hat.wav"),
            _T("sound/horn.wav"),
            _T("sound/hot_dog.wav"),
            _T("sound/iguana.wav"),
            _T("sound/ice_cream.wav"),
            _T("sound/ice_skate.wav"),
            _T("sound/igloo.wav"),
            _T("sound/ipu.wav"),
            _T("sound/jaguar.wav"),
            _T("sound/jar.wav"),
            _T("sound/jug.wav"),
            _T("sound/juice.wav"),
            _T("sound/jump_rope.wav"),
            _T("sound/kazoo.wav"),
            _T("sound/ketchup.wav"),
            _T("sound/keyboard.wav"),
            _T("sound/kite.wav"),
            _T("sound/koala.wav"),
            _T("sound/ladybug.wav"),
            _T("sound/lasagna.wav"),
            _T("sound/lunchbox.wav"),
            _T("sound/lion.wav"),
            _T("sound/lute.wav"),
            _T("sound/magnet.wav"),
            _T("sound/mag_glass.wav"),
            _T("sound/maracas.wav"),
            _T("sound/mouse.wav"),
            _T("sound/mushroom.wav"),
            _T("sound/necklace.wav"),
            _T("sound/newspaper.wav"),
            _T("sound/newt.wav"),
            _T("sound/ney.wav"),
            _T("sound/noodle.wav"),
            _T("sound/oboe.wav"),
            _T("sound/olive.wav"),
            _T("sound/onion.wav"),
            _T("sound/origami.wav"),
            _T("sound/owl.wav"),
            _T("sound/paint.wav"),
            _T("sound/paper_plane.wav"),
            _T("sound/piano.wav"),
            _T("sound/pig.wav"),
            _T("sound/pretzel.wav"),
            _T("sound/quail.wav"),
            _T("sound/quarter.wav"),
            _T("sound/quena.wav"),
            _T("sound/quiche.wav"),
            _T("sound/quilt.wav"),
            _T("sound/raccoon.wav"),
            _T("sound/radishes.wav"),
            _T("sound/recorder.wav"),
            _T("sound/rocking_horse.wav"),
            _T("sound/rooster.wav"),
            _T("sound/sax.wav"),
            _T("sound/sheep.wav"),
            _T("sound/seesaw.wav"),
            _T("sound/star.wav"),
            _T("sound/strawberry.wav"),
            _T("sound/tomato.wav"),
            _T("sound/top.wav"),
            _T("sound/train.wav"),
            _T("sound/trumpet.wav"),
            _T("sound/turtle.wav"),
            _T("sound/udon.wav"),
            _T("sound/ukulele.wav"),
            _T("sound/umbrella.wav"),
            _T("sound/unagi.wav"),
            _T("sound/unicycle.wav"),
            _T("sound/urchin.wav"),
            _T("sound/vase.wav"),
            _T("sound/vegetables.wav"),
            _T("sound/violin.wav"),
            _T("sound/volleyball.wav"),
            _T("sound/vulture.wav"),
            _T("sound/wagon.wav"),
            _T("sound/watch.wav"),
            _T("sound/watermelon.wav"),
            _T("sound/whale.wav"),
            _T("sound/whistle.wav"),
            _T("sound/xcookie.wav"),
            _T("sound/xray.wav"),
            _T("sound/xiphias.wav"),
            _T("sound/xylophone.wav"),
            _T("sound/yak.wav"),
            _T("sound/yam.wav"),
            _T("sound/yangqin.wav"),
            _T("sound/yinyang.wav"),
            _T("sound/yoyo.wav"),
            _T("sound/zebra.wav"),
            _T("sound/zipline.wav"),
            _T("sound/zipper.wav"),
            _T("sound/zucchini.wav"),
            _T("sound/zurna.wav"),
            _T("sound/xun.wav")
        }
    };
    for (int x = 0; x < 131; x += 1) {
        if (sounds[0][x] == name) {
            isplaying = true;
            cur_sound = sounds[1][x];
            rc = pthread_create(&thread1, NULL, OtherThread, this);
            //            if (rc)
            //            {
            //                cout << "Error Occurred" << endl;
            //                exit(-1);
            //            }
        }
    }
}

// Threading

/*void *MainWindow::TabThread(void *arg)
{
    wxEvtHandler *obj = wxDynamicCast(arg, wxEvtHandler);
    if (obj)
    {
        wxCommandEvent TabEvent(ID_TAB, wxID_ANY);
        wxPostEvent(obj, TabEvent);
    }
    pthread_exit(NULL);
}*/

void *MainWindow::SpaceThread(void *arg) {
    wxEvtHandler *obj = wxDynamicCast(arg, wxEvtHandler);
    if (obj) {
    	soundPlayer->load(cur_sound);
    	soundPlayer->play();
        wxCommandEvent SpaceEvent(ID_SPACE, wxID_ANY);
        wxPostEvent(obj, SpaceEvent); // Pass event
    }
    pthread_exit(NULL);
}

void *MainWindow::KeyThread(void *arg) {
    wxEvtHandler *obj = wxDynamicCast(arg, wxEvtHandler);
    if (obj) {
    	soundPlayer->load(cur_sound);
    	soundPlayer->play();
        wxCommandEvent KeyEvent(ID_KEY, wxID_ANY);
        wxPostEvent(obj, KeyEvent);
    }
    pthread_exit(NULL);
}

// Thread used for modes other than ABC, and when space is pressed, to play sound

void *MainWindow::OtherThread(void *arg) {
    wxEvtHandler *obj = wxDynamicCast(arg, wxEvtHandler);
    if (obj) {
    	soundPlayer->load(cur_sound);
    	soundPlayer->play();
        wxCommandEvent OtherEvent(ID_OTHER, wxID_ANY);
        wxPostEvent(obj, OtherEvent);
    }
    pthread_exit(NULL);
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
    wxString CLText(_T(
            "0.4.5\n\u2022 Add .desktop file for X11 desktop\n\u2022 Replace \
some images\n\u2022 Changed license to MIT\n\u2022 Made code non-Linux \
specific"));
    about->SetChangelog(CLText);

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
    about->AddArtist(_T("Vulture(sound)"), _T("free-sound-effects-bird"), _T("AudioMicro Standard License"));
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
    //about->AddArtist(_T("Xun"), _T("Jordan Irwin"), _T("Public Domain"));
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
    bg->SetFocus();
}
