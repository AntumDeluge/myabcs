
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_CREDITSPANEL_H_
#define MYABCS_CREDITSPANEL_H_

#include <wx/scrolwin.h>
#include <wx/window.h>


/** Presents a page for credits in the about dialog */
class CreditsPanel : public wxScrolledWindow {
public:
	CreditsPanel(wxWindow* parent);
	CreditsPanel(wxWindow* parent, int id);
};


#endif /* MYABCS_CREDITSPANEL_H_ */
