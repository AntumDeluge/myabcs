
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "creditspanel.h"


CreditsPanel::CreditsPanel(wxWindow* parent) :
		wxScrolledWindow(parent, wxID_ANY) {}

CreditsPanel::CreditsPanel(wxWindow* parent, int id) :
		wxScrolledWindow(parent, id) {}
