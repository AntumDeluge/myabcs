
/* Copyright © 2010-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "fonts.h"
#include "gnrcabt.h"
#include "id.h"
#include "log.h"
#include "paths.h"
#include "resourceobject.h"
#include "res/logo.h"
#include "res/logo_sdl.h"
#include "res/logo_wx.h"
#include "res/logo_wxsvg.h"

#include <SDL2/SDL_version.h>
#include <wx/ffile.h>
#include <wx/hyperlink.h>
#include <wx/mstream.h>
#include <wx/stattext.h>


#if !defined(WIN32) && !defined(WIN64)
wxIcon ICON1 = wxIcon(myabcs_small_xpm);
#endif

const int ID_INFO = wxNewId();
const int ID_ART = wxNewId();
const int ID_AUDIO = wxNewId();


ABTTextDisplay::ABTTextDisplay(wxWindow* parent, wxWindowID id, wxString label) :
		wxPanel(parent, id) {
	this->label = label;

	text_area = new wxRichTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY);
	if (font_changelog.IsOk()) {
		text_area->SetFont(font_changelog);
	} else {
		logMessage("Custom monospace font not loaded");
	}

	wxBoxSizer *layout = new wxBoxSizer(wxVERTICAL);
	layout->Add(text_area, 1, wxEXPAND);

	SetAutoLayout(true);
	SetSizer(layout);
	Layout();
}

void ABTTextDisplay::setText(wxString text) {
	if (label == "License") {
		// FIXME: will not work for lines that are meant to be single-spaced
		wxArrayString text_array = wxSplit(text, '\n');
		text = wxEmptyString;
		for (size_t idx = 0; idx < text_array.GetCount(); idx++) {
			wxString line = text_array[idx];
			// FIXME: lines with whitespace only will not be considered empty
			if (line == wxEmptyString) {
				line = "\n\n";
			}
			text.Append(line);
		}
	}

	text_area->SetValue(text);
}

void ABTTextDisplay::loadFile(const wxString filename) {
	wxString text = wxString::Format("%s text not found: %s", label, filename);

	if (wxFileExists(filename)) {
		// load & read text file
		wxFFile f_open(filename, "r");
		f_open.ReadAll(&text);
		f_open.Close();
	}

	setText(text);
}


GenericAbout::GenericAbout(wxWindow* parent, wxWindowID id, const wxString& title) :
		wxDialog(parent, id, title) {

	CenterOnParent();

	Bind(wxEVT_INIT_DIALOG, &GenericAbout::onShow, this);

	tabs = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize);

	// TODO: create tabs dynamically
	tab_info = new wxScrolledWindow(tabs, ID_INFO);
	tabs->AddPage(tab_info, "About");
	tab_lic = new ABTTextDisplay(tabs, ID_LICENSE, "License");
	tabs->AddPage(tab_lic, "License");
	tab_log = new ABTTextDisplay(tabs, ID_CHANGELOG, "Changelog");
	tabs->AddPage(tab_log, "Changelog");
	tab_art = new CreditsPanel(tabs, ID_ART);
	tabs->AddPage(tab_art, "Art");
	tab_audio = new CreditsPanel(tabs, ID_AUDIO);
	tabs->AddPage(tab_audio, "Audio");

	iconsize = wxSize(100, 100);
	appicon = new wxStaticBitmap(tab_info, -1, wxNullBitmap, wxDefaultPosition, iconsize);

	infosizer = new wxBoxSizer(wxVERTICAL);
	infosizer->AddSpacer(10);
	infosizer->Add(appicon, 0, wxALIGN_CENTER);
	infosizer->AddSpacer(10);

	tab_info->SetAutoLayout(true);
	tab_info->SetSizer(infosizer);
	tab_info->Layout();

	// button to close dialog
	ok = new wxButton(this, wxID_OK);

	// Layout
	sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(tabs, 1, wxEXPAND);
	sizer->Add(ok, 0, wxALIGN_RIGHT|wxALL, 5);

	SetAutoLayout(true);
	SetSizerAndFit(sizer);
	Layout();
}

void GenericAbout::setImage(wxString image, bool noresize) {
	setImage(wxImage(image), noresize);
}

void GenericAbout::setImage(wxImage image, bool noresize) {
	if (!noresize) {
		// DEBUG:
		logMessage("Resizing logo ...");

		image.Rescale(iconsize.GetWidth(), iconsize.GetHeight(), wxIMAGE_QUALITY_HIGH);
	}
	appicon->SetBitmap(image);

	tab_info->Layout();
}

void GenericAbout::setInfoString(wxString info) {
	wxStaticText* text = new wxStaticText(tab_info, -1, info);
	text->SetFont(wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	infosizer->Add(text, 0, wxALIGN_CENTER);

	tab_info->Layout();
}

void GenericAbout::setLink(wxString label, wxString url) {
	wxHyperlinkCtrl* link = new wxHyperlinkCtrl(tab_info, -1, label, url);
	infosizer->Add(link, 0, wxALIGN_CENTER|wxBOTTOM, 5);

	tab_info->Layout();
}

void GenericAbout::setCopyright(wxString copyright) {
	wxStaticText* text = new wxStaticText(tab_info, -1, copyright);
	text->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	infosizer->Add(text, 0, wxALIGN_CENTER|wxBOTTOM, 5);

	tab_info->Layout();
}

void GenericAbout::setAbout(wxString about) {
	wxStaticText* text = new wxStaticText(tab_info, -1, about);
	infosizer->Add(text, 1, wxALIGN_CENTER|wxBOTTOM, 5);

	tab_info->Layout();
}

/**
 * TODO: support links
 *
 * @param name
 * @param author
 * @param license
 */
void GenericAbout::addArtist(wxString name, wxString author, wxString license) {
	tab_art->add(name, author, wxEmptyString, license, wxEmptyString);
}

/**
 * TODO: support links
 *
 * @param name
 * @param author
 * @param license
 */
void GenericAbout::addComposer(wxString name, wxString author, wxString license) {
	tab_audio->add(name, author, wxEmptyString, license, wxEmptyString);
}

void GenericAbout::setLicense(wxString filename) {
	tab_lic->loadFile(filename);
}

void GenericAbout::setChangelog(wxString filename) {
	tab_log->loadFile(filename);
}

void GenericAbout::addToolkitInfo() {
	infosizer->AddSpacer(10);
	infosizer->Add(new wxStaticText(tab_info, -1, "Made with:"), 0, wxALIGN_CENTER);
	infosizer->AddSpacer(10);

	SDL_version sdl_v;
	SDL_VERSION(&sdl_v);
	const wxString wx_version = wxString::Format("v%s", wxSplit(wxVERSION_STRING, ' ')[1]);
	const wxString sdl_version = wxString::Format("v%d.%d.%d", sdl_v.major, sdl_v.minor, sdl_v.patch);

	wxStaticBitmap* wx_logo = new wxStaticBitmap(tab_info, -1, wxBitmap(imageFromSVG(wxwidgets_svg, sizeof(wxwidgets_svg), 75, 75)));
	wxStaticBitmap* sdl_logo = new wxStaticBitmap(tab_info, -1, wxBitmap(imageFromSVG(sdl_svg, sizeof(sdl_svg), 75, 75)));
	wxStaticBitmap* wxsvg_logo = new wxStaticBitmap(tab_info, -1, wxBitmap(imageFromSVG(wxsvg_svg, sizeof(wxsvg_svg), 75, 75)));

	wxHyperlinkCtrl* wx_link = new wxHyperlinkCtrl(tab_info, -1, "wxWidgets", "https://www.wxwidgets.org/");
	wxHyperlinkCtrl* sdl_link = new wxHyperlinkCtrl(tab_info, -1, "Simple DirectMedia Layer", "https://libsdl.org/");
	wxHyperlinkCtrl* wxsvg_link = new wxHyperlinkCtrl(tab_info, -1, "wxSVG", "http://wxsvg.sourceforge.net/");

	// TODO: Add SDL_mixer information (maybe)
	wxFlexGridSizer* tk_layout = new wxFlexGridSizer(6, 2, 10, 10);
	tk_layout->Add(wx_logo, 0, wxALIGN_CENTER);
	tk_layout->Add(sdl_logo, 0, wxALIGN_CENTER);
	tk_layout->Add(wx_link, 0, wxALIGN_CENTER);
	tk_layout->Add(sdl_link, 0, wxALIGN_CENTER);
	tk_layout->Add(new wxStaticText(tab_info, -1, wx_version), 0, wxALIGN_CENTER);
	tk_layout->Add(new wxStaticText(tab_info, -1, sdl_version), 0, wxALIGN_CENTER);
	tk_layout->Add(wxsvg_logo, 0, wxALIGN_CENTER);
	tk_layout->AddStretchSpacer();
	tk_layout->Add(wxsvg_link, 0, wxALIGN_CENTER);
	tk_layout->AddStretchSpacer();

#ifdef WXSVG_VERSION
	tk_layout->Add(new wxStaticText(tab_info, -1, wxString::Format("v%s", WXSVG_VERSION)), 0, wxALIGN_CENTER);
#endif

	infosizer->Add(tk_layout, 0, wxALIGN_CENTER);
}

void GenericAbout::addMiscInfo() {
	// TODO: parse Markdown README
	wxPanel* tab_misc = new wxPanel(tabs);

	wxHyperlinkCtrl* font_main = new wxHyperlinkCtrl(tab_misc, -1, "Sniglet", "https://fontlibrary.org/font/sniglet");
	wxHyperlinkCtrl* font_mono = new wxHyperlinkCtrl(tab_misc, -1, "Pixel Operator Mono", "https://fontlibrary.org/font/sniglet");
	font_main->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Sniglet"));
	font_mono->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Pixel Operator Mono"));

	wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);

	layout->Add(new wxStaticText(tab_misc, -1, "Fonts used in this software:"), 0, wxALIGN_CENTER|wxTOP, 20);
	layout->Add(font_main, 0, wxALIGN_CENTER|wxTOP, 10);
	layout->Add(font_mono, 0, wxALIGN_CENTER|wxTOP, 10);

	tab_misc->SetSizer(layout);
	tabs->AddPage(tab_misc, "Misc");
}

/**
 * @override wxDialog::ShowModal
 * @return
 */
int GenericAbout::ShowModal() {
	// set vertical scrollbar only
	tab_info->SetScrollbars(0, 20, 0, 50);
	Fit();

	// retain width
	SetSize(GetSize().GetWidth(), 350);

	return wxDialog::ShowModal();
}


// private methods

/** Shows first tab & centers dialog on main window.
 *
 * @param event
 */
void GenericAbout::onShow(wxEvent& event) {
	CenterOnParent();
	ok->SetFocus();
	tabs->ChangeSelection(0);

	// TODO: bind event to make implicit call
	tab_art->onShow(event);
	tab_audio->onShow(event);
}


// public functions

static GenericAbout* about;
static bool initialized = false;


void initAboutDialog(wxWindow* parent) {
	about = new GenericAbout(parent);

	about->SetIcon(ICON1);
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

	about->setLicense(getRootDir().Append("/LICENSE.txt"));
	about->setChangelog(getRootDir().Append("/CHANGES.txt"));

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
	about->addArtist("Doughnut", "OCAL", "CC0/Public Domain"); // https://openclipart.org/detail/279100
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
	about->addArtist("Ice Cream", "Paul Sherman", "CC0"); // https://www.wpclipart.com/food/desserts_snacks/ice_cream/sundea/ice_cream_sundae.png.html
	about->addArtist("Ice Skate", "Francesco Rollandin", "Public Domain");
	about->addArtist("Igloo", "Jose Hevia", "Public Domain");
	about->addArtist("Iguana", wxEmptyString, "Public Domain"); // https://www.wpclipart.com/animals/L/lizard/iguana/iguana.png.html
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
	about->addArtist("Owl", wxEmptyString, "Public Domain"); // https://www.wpclipart.com/animals/birds/O/owl/Owl_4/eagle_owl.png.html
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
	about->addArtist("Rooster", "andinuryadin", "CC0"); // https://openclipart.org/detail/175282/ayam-jantan | https://commons.wikimedia.org/wiki/File:Rooster_by_andinuryadin.svg
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
	about->addArtist("Vegetables", "Paul Sherman", "CC0"); // https://www.wpclipart.com/food/vegetables/vegetable_basket.png.html
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

	about->addComposer("Accordion", "alphatone", "CC BY"); // https://freesound.org/people/alphatone/sounds/385876/
	about->addComposer("Airplane", "Manuel Fidalgo (granizo)", "CC BY"); // https://freesound.org/people/granizo/sounds/401157/
	about->addComposer("Bagpipes", "kyles", "CC0/Public Domain"); // https://freesound.org/people/kyles/sounds/453749/
	about->addComposer("Balloons", "Gniffelbaf & DNAudio_UK", "CC0"); // https://freesound.org/people/Gniffelbaf/sounds/82119/, https://freesound.org/people/DNAudio_UK/sounds/263809/
	about->addComposer("Bird", "Jc Guan", "Public Domain"); // http://soundbible.com/340-Bird-Song.html
	about->addComposer("Bicycle", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16320
	about->addComposer("Cat", "blimp66", "CC BY"); // https://freesound.org/people/blimp66/sounds/397661/
	about->addComposer("Chalkboard", "KCJones321", "CC0"); // https://freesound.org/people/KCJones321/sounds/411205/
	about->addComposer("Clarinet", "Serolillo", "CC BY 2.5"); // https://commons.wikimedia.org/wiki/File:Jazz_Clarinet.ogg
	about->addComposer("Clock", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16323
	about->addComposer("Dog", "ivolipa", "CC0"); // https://freesound.org/people/ivolipa/sounds/328730/
	about->addComposer("Dolphin", "Nicholas Judy Sound FX Studios", "CC BY"); // https://www.youtube.com/watch?v=_EoRq84vtTo
	about->addComposer("Drums", "imakepitart", "Public Domain"); // https://commons.wikimedia.org/wiki/File:Drums.ogg (originally from pdsounds.org)
	about->addComposer("Electric Guitar", "Mattgirling", "Attribution-ShareAlike 3.0"); // https://commons.wikimedia.org/wiki/File:GuitarRig4.ogg
	about->addComposer("Elephant", "vataaa", "CC0"); // https://freesound.org/people/vataaa/sounds/148873/
	about->addComposer("err", "reelworldstudio", "CC0"); // https://freesound.org/people/reelworldstudio/sounds/161122/
	about->addComposer("Fire", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16327
	about->addComposer("Flute", "juskiddink", "CC BY"); // https://freesound.org/people/juskiddink/sounds/65510/
	about->addComposer("Frog", "kayceemixer", "CC0"); // https://freesound.org/people/kayceemixer/sounds/251495/
	about->addComposer("Giraffe", "Anton Baotic, Florian Sicks & Angela S. Stoeger", "CC BY-SA"); // https://bmcresnotes.biomedcentral.com/articles/10.1186/s13104-015-1394-3
	about->addComposer("Grasshopper", "straget", "CC0/Public Domain"); // https://freesound.org/people/straget/sounds/401939/
	about->addComposer("Guitar", "Jordan Irwin", "CC0"); // https://opengameart.org/node/98720
	about->addComposer("Harp", "adriann", "CC0"); // https://freesound.org/people/adriann/sounds/149187/
	about->addComposer("Horn(sound", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16322
	about->addComposer("Horse", "GoodListener", "CC BY"); // https://freesound.org/people/GoodListener/sounds/322443/
	about->addComposer("Ipu", "Stumber", "CC0"); // https://freesound.org/people/Stumber/sounds/207863/
	about->addComposer("Jaguar", "About Zoos", "CC BY"); // https://www.youtube.com/watch?v=VJ0RCZXu0v4
	about->addComposer("Jug", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16325
	about->addComposer("Kazoo", "NoiseCollector", "CC BY"); // https://freesound.org/people/NoiseCollector/sounds/7781/
	about->addComposer("Keyboard", "Trollarch2", "CC0/Public Domain"); // https://freesound.org/people/Trollarch2/sounds/331656/
	about->addComposer("Koala", "Nicholas Judy Sound FX Studios", "CC BY"); // https://www.youtube.com/watch?v=iIFWGX4-0RM
	about->addComposer("Lion", "felix.blume", "CC0"); // https://freesound.org/people/felix.blume/sounds/405211/
	about->addComposer("Maracas", "Wikipedia-ce", "Public Domain"); // https://commons.wikimedia.org/wiki/File:Maracas.ogg
	about->addComposer("Mouse", "Jordan Irwin", "CC0"); // https://opengameart.org/node/16331
	about->addComposer("Oboe", "acclivity", "CC BY-NC"); // https://freesound.org/people/acclivity/sounds/22686/
	about->addComposer("Owl", "Anthousai", "CC0"); // https://freesound.org/people/Anthousai/sounds/398734/
	about->addComposer("Piano", "Scott Joplin / Tim Starling", "Public Domain"); // https://commons.wikimedia.org/wiki/File:Maple_leaf_rag.ogg
	about->addComposer("Pig", "Robinhood76", "CC BY-NC"); // https://freesound.org/people/Robinhood76/sounds/76796/
	about->addComposer("Quail", "PrincessGrace", "CC0"); // https://freesound.org/people/PrincessGrace/sounds/329371/
	about->addComposer("Quarter", "greenvwbeetle", "CC0/Public Domain"); // https://freesound.org/people/greenvwbeetle/sounds/423332/
	about->addComposer("Quena", "CarlosCarty", "CC BY"); // https://freesound.org/people/CarlosCarty/sounds/463711/
	about->addComposer("Raccoon", "jnargus", "CC BY"); // https://www.youtube.com/watch?v=BGjFP1CP7E0
	about->addComposer("Recorder", "Cailyn");
	about->addComposer("Rocking Horse", "stevelalonde", "CC BY"); // https://freesound.org/people/stevelalonde/sounds/80476/
	about->addComposer("Rooster", "poorenglishjuggler", "CC0/Public Domain"); // https://freesound.org/people/poorenglishjuggler/sounds/269496/
	about->addComposer("Saxophone", "Sylenius", "Public Domain"); // https://en.wikipedia.org/wiki/File:Reed_phase.ogg
	about->addComposer("Sheep", "mikewest", "CC0/Public Domain"); // https://freesound.org/people/mikewest/sounds/414342/
	about->addComposer("Train", "Daniel Simion", "CC BY"); // http://soundbible.com/2177-Steam-Train-Whistle.html
	about->addComposer("Trumpet ", "sorohanro", "CC BY"); // https://freesound.org/people/sorohanro/sounds/77706/
	about->addComposer("Ukulele", "Henry Kailimai", "Attribution-ShareAlike 3.0"); // https://commons.wikimedia.org/wiki/File:Ukelele_-_Kailimai%27s_hene_-_Ukepedia.ogg
	about->addComposer("Umbrella", "j1987", "CC0/Public Domain"); // https://freesound.org/people/j1987/sounds/73028/
	about->addComposer("Violin", _T("Edit Kov\u00E1cs"), "Attribution-ShareAlike 2.0 Germany"); // https://commons.wikimedia.org/wiki/File:Whiskey-youre-the-devil-violin.ogg
	about->addComposer("Vulture", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16329
	about->addComposer("Watch", "Nicolas4677", "CC0/Public Domain"); // https://freesound.org/people/Nicolas4677/sounds/446611/
	about->addComposer("Whale", "copyrighy free sound-effect", "CC BY"); // https://www.youtube.com/watch?v=ZYwSxoRAkuQ
	about->addComposer("Whistle", "pawsound", "CC0/Public Domain"); // https://freesound.org/people/pawsound/sounds/154873/
	about->addComposer("Xun", "franeknflute", "CC BY"); // https://freesound.org/people/franeknflute/sounds/200419/
	about->addComposer("Xylophone", "Tristan", "CC0"); // https://freesound.org/people/Tristan/sounds/19459/
	about->addComposer("Yak", "Jordan Irwin", "CC0"); // https://opengameart.org/node/98682
	about->addComposer("Yangqin", "matiasromero", "CC0"); // https://freesound.org/people/matiasromero/sounds/22261/
	about->addComposer("Zebra", "Peet J van Eeden", "CC BY"); // https://www.youtube.com/watch?v=xjGJ_XMVTRE
	about->addComposer("Zipper", "Jordan Irwin", "CC0/Public Domain"); // https://opengameart.org/node/16332
	about->addComposer("Zurna", "cdrk", "CC BY"); // https://freesound.org/people/cdrk/sounds/18661/

	about->addToolkitInfo();
	about->addMiscInfo();

	// CenterOnParent is called in the constructor}

	initialized = true;
}


int showAboutDialog(wxWindow* parent) {
	if (!initialized) {
		initAboutDialog(parent);
	}

	return about->ShowModal();
}
