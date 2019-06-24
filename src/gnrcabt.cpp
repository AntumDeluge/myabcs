#include "gnrcabt.h"
#include "id.h"

GenericAbout::GenericAbout(wxWindow* parent, wxWindowID id, const wxString& title) : wxDialog(parent, id, title)
{
	CenterOnParent();

	Connect(wxEVT_INIT_DIALOG, wxEventHandler(GenericAbout::OnShow), 0, this);

	tabs = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize);
	p1_info = new wxPanel(tabs, ID_INFO);
	p2_credits = new wxScrolledWindow(tabs, wxID_ANY);
	p3_art = new wxPanel(tabs, ID_ART);
	p4_log = new wxPanel(tabs, ID_CHANGELOG);
	tabs->AddPage(p1_info, _T("About"));
	tabs->AddPage(p2_credits, _T("Credits"));

	iconsize = new wxSize();
	// FIXME: fonts
	const wxFont font1(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	const wxFont font2(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

	appicon = new wxStaticBitmap(p1_info, -1, wxNullBitmap, wxDefaultPosition, wxSize(100,100));
	appname = new wxStaticText(p1_info, -1, wxEmptyString);
	appname->SetFont(font1);
	appver = new wxStaticText(p1_info, -1, wxEmptyString);
	appver->SetFont(font1);
	appcopyright = new wxStaticText(p1_info, -1, wxEmptyString);
	appcopyright->SetFont(font2);
	appurl = new wxHyperlinkCtrl(p1_info, -1, _T("myabcs.sourceforge.net"),
			_T("http://myabcs.sourceforge.net/"));
	appabout = new wxStaticText(p1_info, -1, wxEmptyString);

	wxBoxSizer *namesizer = new wxBoxSizer(wxHORIZONTAL);
	namesizer->Add(appname);
	namesizer->AddSpacer(10);
	namesizer->Add(appver);

	infosizer = new wxBoxSizer(wxVERTICAL);
	infosizer->AddSpacer(10);
	infosizer->Add(appicon, 0, wxALIGN_CENTER);
	infosizer->AddSpacer(10);
	infosizer->Add(namesizer, 0, wxALIGN_CENTER);
	infosizer->Add(appcopyright, 0, wxALIGN_CENTER|wxBOTTOM, 5);
	infosizer->Add(appurl, 0, wxALIGN_CENTER|wxBOTTOM, 5);
	infosizer->Add(appabout, 1, wxALIGN_CENTER|wxBOTTOM, 5);

	p1_info->SetAutoLayout(true);
	p1_info->SetSizer(infosizer);
	p1_info->Layout();

	// Credits
	devtext = new wxStaticText(p2_credits, -1, _T("Developers"));
	devbox = new gaListBox(p2_credits, -1);
	packtext = new wxStaticText(p2_credits, -1, _T("Packagers"));
	packbox = new gaListBox(p2_credits, -1);
	transtext = new wxStaticText(p2_credits, -1, _T("Translators"));
	transbox = new gaListBox(p2_credits, -1);
	arttext = new wxStaticText(p2_credits, -1, _T("Artists"));
	artbox = new gaListBox(p2_credits, -1);

	wxBoxSizer *devsizer = new wxBoxSizer(wxHORIZONTAL);
	devsizer->Add(devtext, 1);
	devsizer->Add(devbox, 2);
	wxBoxSizer *packsizer = new wxBoxSizer(wxHORIZONTAL);
	packsizer->Add(packtext, 1);
	packsizer->Add(packbox, 2);
	wxBoxSizer *transsizer = new wxBoxSizer(wxHORIZONTAL);
	transsizer->Add(transtext, 1);
	transsizer->Add(transbox, 2);
	wxBoxSizer *artistsizer = new wxBoxSizer(wxHORIZONTAL);
	artistsizer->Add(arttext, 1);
	artistsizer->Add(artbox, 2);

	wxBoxSizer *creditsizer = new wxBoxSizer(wxVERTICAL);
	creditsizer->Add(devsizer, 1, wxEXPAND);
	creditsizer->Add(packsizer, 1, wxEXPAND);
	creditsizer->Add(transsizer, 1, wxEXPAND);
	creditsizer->Add(artistsizer, 1, wxEXPAND);

	p2_credits->SetAutoLayout(true);
	p2_credits->SetSizer(creditsizer);
	p2_credits->Layout();

	// Art
	artists = new wxListCtrl(p3_art, -1, wxDefaultPosition, wxSize(300,200), wxLC_REPORT|wxNO_BORDER);

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

	p3_art->SetAutoLayout(true);
	p3_art->SetSizer(artsizer);
	p3_art->Layout();

	//artists->Connect(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, wxListEventHandler(GenericAbout::CancelColResize), 0, this);

	// Changelog
	changelog = new wxRichTextCtrl(p4_log, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY);

	wxBoxSizer *logsizer = new wxBoxSizer(wxVERTICAL);
	logsizer->Add(changelog, 1, wxEXPAND);

	p4_log->SetAutoLayout(true);
	p4_log->SetSizer(logsizer);
	p4_log->Layout();

	// OK button
	ok = new wxButton(this, wxID_OK);

	// Layout
	sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(tabs, 1, wxEXPAND);
	sizer->Add(ok, 0, wxALIGN_RIGHT|wxALL, 5);

	SetAutoLayout(true);
	SetSizerAndFit(sizer);
	Layout();

	*iconsize = sizer->GetSize();
	appicon->SetSize(iconsize->GetWidth()/2, iconsize->GetWidth()/2);
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

void GenericAbout::SetImage(wxString image)
{
	wxImage newicon(image, wxBITMAP_TYPE_ANY);
	newicon.Rescale(100, 100, wxIMAGE_QUALITY_HIGH);
	appicon->SetBitmap(newicon);

	p1_info->Layout();
}

void GenericAbout::SetImage(wxIcon image)
{
	wxBitmap bmpicon(image);
	wxImage newicon(bmpicon.ConvertToImage());
	newicon.Rescale(100,100, wxIMAGE_QUALITY_HIGH);
	appicon->SetBitmap(image);

	p1_info->Layout();
}

void GenericAbout::SetName(wxString name)
{
	appname->SetLabel(name);
}

void GenericAbout::SetVersion(wxString version)
{
	appver->SetLabel(version);
}

void GenericAbout::SetCopyright(wxString copyright)
{
	appcopyright->SetLabel(copyright);
}

void GenericAbout::SetAbout(wxString about)
{
	appabout->SetLabel(about);
}

void GenericAbout::AddCredit(wxString name, const int credit_type)
{
	if (credit_type == CREDIT_DEVELOPER) devbox->Append(name);
	else if (credit_type == CREDIT_PACKAGER) packbox->Append(name);
	else if (credit_type == CREDIT_TRANSLATOR) transbox->Append(name);
	else if (credit_type == CREDIT_ARTIST) artbox->Append(name);
}

/*void GenericAbout::AddCredit(wxArrayString& names, int credit_type);
{
	if (credit_type == CREDIT_DEVELOPER) devbox->InsertItems(names);
	else if (credit_type == CREDIT_PACKAGER) packbox->InsertItems(names);
	else if (credit_type == CREDIT_TRANSLATOR) transbox->InsertItems(names);
	else if (credit_type == CREDIT_ARTIST) artbox->InsertItems(names);
}*/

void GenericAbout::AddArtist(wxString image, wxString name, wxString license)
{
	// FIXME: Index error
	if (tabs->GetPage(2) != p3_art)
	{
		tabs->InsertPage(2, p3_art, _T("Art"));
		SetSizerAndFit(sizer);
		Layout();
	}

	int count = artists->GetItemCount();

	artists->InsertItem(count, image);
	artists->SetItem(count, 1, name);
	artists->SetItem(count, 2, license);
}

void GenericAbout::SetChangelog(wxString log)
{
	changelog->SetValue(log);
	tabs->AddPage(p4_log, _T("Changelog"));
}

gaListBox::gaListBox(wxWindow* parent, wxWindowID id) : wxListBox(parent, id=wxID_ANY)
{
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(gaListBox::VoidSelect), 0, this);
}

void gaListBox::VoidSelect(wxMouseEvent& event)
{
}
