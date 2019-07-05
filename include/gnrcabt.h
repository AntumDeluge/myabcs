#ifndef MYABCS_GNRCABT_H
#define MYABCS_GNRCABT_H

#include "creditspanel.h"

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
	void AddArtist(wxString image=wxEmptyString, wxString name=wxEmptyString, wxString license=wxEmptyString);
	void SetChangelog(wxString log);
private:
	void OnShow(wxEvent& event);
	wxNotebook* tabs;
	wxButton* ok;
	wxPanel* tab_info;
	CreditsPanel* tab_art;
	wxPanel* tab_log;

	// Info
	wxStaticBitmap* appicon;
	wxSize* iconsize;
	wxStaticText* appname;
	wxStaticText* appver;
	wxStaticText* appcopyright;
	wxHyperlinkCtrl* appurl;
	wxStaticText* appabout;

	wxBoxSizer* infosizer;

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
