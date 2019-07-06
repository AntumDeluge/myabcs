
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_MAIN_H
#define MYABCS_MAIN_H

#include <wx/app.h>

class App : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

#endif /* MYABCS_MAIN_H */
