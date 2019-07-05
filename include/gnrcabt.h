#ifndef MYABCS_GNRCABT_H
#define MYABCS_GNRCABT_H

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/hyperlink.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>


// FIXME: these should not be defined here
const int ID_INFO = wxNewId();
const int ID_ART = wxNewId();
const int CREDIT_DEVELOPER = 100;
const int CREDIT_TRANSLATOR = 101;
const int CREDIT_ARTIST = 102;
const int CREDIT_PACKAGER = 103;


class GenericAbout : public wxDialog {
public:
	GenericAbout(wxWindow* parent, wxWindowID id, const wxString& title=_T("About"));
	void SetImage(wxString image);
	void SetImage(wxIcon image);
	void SetName(wxString name);
	void SetVersion(wxString version);
	void SetCopyright(wxString copyright);
	void SetURL(wxString url);
	void SetAbout(wxString about);
	void AddCredit(wxString name, int credit_type);
	//void AddCredit(wxArrayString& names, int credit_type);
	void AddArtist(wxString image=wxEmptyString, wxString name=wxEmptyString, wxString license=wxEmptyString);
	void SetChangelog(wxString log);
private:
	void OnShow(wxEvent& event);
	wxNotebook* tabs;
	wxButton* ok;
	wxPanel* p1_info;
	wxScrolledWindow* p2_credits;
	wxPanel* p3_art;
	wxPanel* p4_log;

	// Info
	wxStaticBitmap* appicon;
	wxSize* iconsize;
	wxStaticText* appname;
	wxStaticText* appver;
	wxStaticText* appcopyright;
	wxHyperlinkCtrl* appurl;
	wxStaticText* appabout;

	wxBoxSizer* infosizer;

	// Credits
	wxStaticText* devtext;
	wxListBox* devbox;
	wxStaticText* packtext;
	wxListBox* packbox;
	wxStaticText* transtext;
	wxListBox* transbox;
	wxStaticText* arttext;
	wxListBox* artbox;

	// Art
	wxListCtrl* artists;
	void CancelColResize(wxListEvent& event);

	// Changelog
	wxRichTextCtrl* changelog;

	wxBoxSizer* sizer;
};

// XXX: unused class?
class gaListBox : public wxListBox {
public:
	gaListBox(wxWindow* parent, wxWindowID id);
	void VoidSelect(wxMouseEvent& event);
};

#endif /* MYABCS_GNRCABT_H */
