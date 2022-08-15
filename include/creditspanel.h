
/* Copyright © 2010-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_CREDITSPANEL_H_
#define MYABCS_CREDITSPANEL_H_

#include <wx/listctrl.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/window.h>


/** Presents a page for credits in the about dialog
 *
 * FIXME: doesn't need to be derived from wxScrolledWindow if using wxListCtrl
 */
class CreditsPanel : public wxScrolledWindow {
public:
	CreditsPanel(wxWindow* parent);
	CreditsPanel(wxWindow* parent, int id);
	void add(wxString name, wxString author, wxString author_url, wxString license, wxString license_url);
	void onShow(wxEvent& event);
private:
	wxBoxSizer* main_layout;
	wxListCtrl* list_items;

	void initList();
	void cancelColResize(wxListEvent& event);
};


#endif /* MYABCS_CREDITSPANEL_H_ */
