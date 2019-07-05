#include "fonts.h"
#include "gnrcabt.h"
#include "id.h"
#include "log.h"

#include <wx/hyperlink.h>
#include <wx/stattext.h>


GenericAbout::GenericAbout(wxWindow* parent, wxWindowID id, const wxString& title) :
		wxDialog(parent, id, title) {
	CenterOnParent();

	Connect(wxEVT_INIT_DIALOG, wxEventHandler(GenericAbout::OnShow), 0, this);

	tabs = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize);

	// TODO: create tabs dynamically
	tab_info = new wxPanel(tabs, ID_INFO);
	tabs->AddPage(tab_info, "About");
	tab_art = new CreditsPanel(tabs, ID_ART);
	tabs->AddPage(tab_art, "Art");
	tab_log = new wxPanel(tabs, ID_CHANGELOG);
	tabs->AddPage(tab_log, "Changelog");

	iconsize = wxSize(100, 100);
	appicon = new wxStaticBitmap(tab_info, -1, wxNullBitmap, wxDefaultPosition, iconsize);

	infosizer = new wxBoxSizer(wxVERTICAL);
	infosizer->AddSpacer(10);
	infosizer->Add(appicon, 0, wxALIGN_CENTER);
	infosizer->AddSpacer(10);

	tab_info->SetAutoLayout(true);
	tab_info->SetSizer(infosizer);
	tab_info->Layout();

	// Art
	artists = new wxListCtrl(tab_art, -1, wxDefaultPosition, wxSize(300,200), wxLC_REPORT|wxNO_BORDER);

	wxSize artsize = artists->GetSize();
	int colwidth = artsize.GetWidth()/3;

	artists->InsertColumn(0, _T("Media"));
	artists->SetColumnWidth(0, colwidth);
	artists->InsertColumn(1, _T("Artist"));
	artists->SetColumnWidth(1, colwidth);
	artists->InsertColumn(2, _T("License"));
	artists->SetColumnWidth(2, colwidth*2);

	wxBoxSizer *artsizer = new wxBoxSizer(wxVERTICAL);
	artsizer->Add(artists, 1, wxEXPAND);

	tab_art->SetAutoLayout(true);
	tab_art->SetSizer(artsizer);
	tab_art->Layout();

	//artists->Connect(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, wxListEventHandler(GenericAbout::CancelColResize), 0, this);

	// Changelog
	changelog = new wxRichTextCtrl(tab_log, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY);
	if (font_changelog.IsOk()) {
		changelog->SetFont(font_changelog);
	} else {
		logMessage("Custom changelog font not loaded");
	}

	wxBoxSizer *logsizer = new wxBoxSizer(wxVERTICAL);
	logsizer->Add(changelog, 1, wxEXPAND);

	tab_log->SetAutoLayout(true);
	tab_log->SetSizer(logsizer);
	tab_log->Layout();

	// OK button
	ok = new wxButton(this, wxID_OK);

	// Layout
	sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(tabs, 1, wxEXPAND);
	sizer->Add(ok, 0, wxALIGN_RIGHT|wxALL, 5);

	SetAutoLayout(true);
	SetSizerAndFit(sizer);
	Layout();
}

void GenericAbout::OnShow(wxEvent& event)
{
	CenterOnParent();

	ok->SetFocus();

	tabs->ChangeSelection(0);
}

void GenericAbout::CancelColResize(wxListEvent& event)
{
	event.Veto();
}

void GenericAbout::SetImage(wxString image) {
	wxImage new_image(image);
	new_image.Rescale(iconsize.GetWidth(), iconsize.GetHeight(), wxIMAGE_QUALITY_HIGH);
	appicon->SetBitmap(new_image);

	tab_info->Layout();
}

void GenericAbout::SetImage(wxImage image) {
	image.Rescale(iconsize.GetWidth(), iconsize.GetHeight(), wxIMAGE_QUALITY_HIGH);
	appicon->SetBitmap(image);

	tab_info->Layout();
}

void GenericAbout::SetInfoString(wxString info) {
	wxStaticText* text = new wxStaticText(tab_info, -1, info);
	text->SetFont(wxFont(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	infosizer->Add(text, 0, wxALIGN_CENTER);

	tab_info->Layout();
}

void GenericAbout::SetLink(wxString label, wxString url) {
	wxHyperlinkCtrl* link = new wxHyperlinkCtrl(tab_info, -1, label, url);
	infosizer->Add(link, 0, wxALIGN_CENTER|wxBOTTOM, 5);

	tab_info->Layout();
}

void GenericAbout::SetCopyright(wxString copyright) {
	wxStaticText* text = new wxStaticText(tab_info, -1, copyright);
	text->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	infosizer->Add(text, 0, wxALIGN_CENTER|wxBOTTOM, 5);

	tab_info->Layout();
}

void GenericAbout::SetAbout(wxString about) {
	wxStaticText* text = new wxStaticText(tab_info, -1, about);
	infosizer->Add(text, 1, wxALIGN_CENTER|wxBOTTOM, 5);

	tab_info->Layout();
}

void GenericAbout::AddArtist(wxString image, wxString name, wxString license) {
	int count = artists->GetItemCount();

	artists->InsertItem(count, image);
	artists->SetItem(count, 1, name);
	artists->SetItem(count, 2, license);
}

void GenericAbout::SetChangelog(wxString log)
{
	changelog->SetValue(log);
}

gaListBox::gaListBox(wxWindow* parent, wxWindowID id) :
		wxListBox(parent, id=wxID_ANY) {
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(gaListBox::VoidSelect), 0, this);
}

void gaListBox::VoidSelect(wxMouseEvent& event) {}
