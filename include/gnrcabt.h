#ifndef MYABCS_GNRCABT_H
#define MYABCS_GNRCABT_H

#include "creditspanel.h"

#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/image.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>


class GenericAbout : public wxDialog {
public:
	GenericAbout(wxWindow* parent, wxWindowID id, const wxString& title=_T("About"));
	void setImage(wxString image);
	void setImage(wxImage image);
	wxSize getImageSize() { return iconsize; }
	void setInfoString(wxString info);
	void setCopyright(wxString copyright);
	void setLink(wxString label, wxString url);
	void setAbout(wxString about);
	void addArtist(wxString image=wxEmptyString, wxString name=wxEmptyString, wxString license=wxEmptyString);
	void addComposer(wxString image=wxEmptyString, wxString name=wxEmptyString, wxString license=wxEmptyString);
	void setChangelog(wxString log);
	void initToolkitInfo();
	virtual int ShowModal();
private:
	wxNotebook* tabs;
	wxButton* ok;
	wxScrolledWindow* tab_info;
	CreditsPanel* tab_art;
	CreditsPanel* tab_audio;
	wxPanel* tab_log;

	// Info
	wxStaticBitmap* appicon;
	wxSize iconsize;
	wxBoxSizer* infosizer;

	// Changelog
	wxRichTextCtrl* changelog;

	wxBoxSizer* sizer;

	void onShow(wxEvent& event);
};

#endif /* MYABCS_GNRCABT_H */
