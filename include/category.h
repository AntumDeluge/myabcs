
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_CATEGORY_H
#define MYABCS_CATEGORY_H

#include "resourcelist.h"

#include <string>
#include <wx/string.h>


extern ResourceList createCategory(std::string cat_name);

extern ResourceList createCategory(wxString cat_name);


#endif /* MYABCS_CATEGORY_H */
