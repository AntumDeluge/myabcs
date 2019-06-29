#ifndef MYABCS_CATEGORY_H
#define MYABCS_CATEGORY_H

#include "resourcelist.h"

#include <string>
#include <wx/string.h>


extern ResourceList createCategory(std::string cat_name);

extern ResourceList createCategory(wxString cat_name);


#endif /* MYABCS_CATEGORY_H */
