
/* Copyright © 2010-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "creditspanel.h"


CreditsPanel::CreditsPanel(wxWindow* parent) :
		wxScrolledWindow(parent, wxID_ANY) {

	main_layout = new wxBoxSizer(wxHORIZONTAL);
	SetAutoLayout(true);
	SetSizer(main_layout);

	initList();
}

CreditsPanel::CreditsPanel(wxWindow* parent, int id) :
		wxScrolledWindow(parent, id) {

	main_layout = new wxBoxSizer(wxHORIZONTAL);
	SetAutoLayout(true);
	SetSizer(main_layout);

	initList();
}

void CreditsPanel::add(wxString name, wxString author, wxString author_url, wxString license, wxString license_url) {
	const unsigned int count = list_items->GetItemCount();

	list_items->InsertItem(count, name);
	list_items->SetItem(count, 1, author);
	list_items->SetItem(count, 2, license);
}

/** Resets columns' width.
 *
 * TODO: connect event to do automatically
 *
 * @param event
 */
void CreditsPanel::onShow(wxEvent& event) {
	const int colwidth = list_items->GetSize().GetWidth() / list_items->GetColumnCount();

	list_items->SetColumnWidth(0, colwidth);
	list_items->SetColumnWidth(1, colwidth);
	list_items->SetColumnWidth(2, colwidth*2);
}


// private methods

void CreditsPanel::initList() {
	list_items = new wxListCtrl(this, -1, wxDefaultPosition, wxSize(300,200), wxLC_REPORT|wxNO_BORDER);

	//list_items->Bind(wxEVT_COMMAND_LIST_COL_BEGIN_DRAG, &GenericAbout::cancelColResize), this);

	list_items->InsertColumn(0, "Media");
	list_items->InsertColumn(1, "Artist");
	list_items->InsertColumn(2, "License");

	main_layout->Add(list_items, 1, wxEXPAND);
}

void CreditsPanel::cancelColResize(wxListEvent& event) {
	event.Veto();
}
