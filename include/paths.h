
/* Copyright © 2010-2022 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_PATHS_H
#define MYABCS_PATHS_H

#include <wx/string.h>

// XXX: any benefit to using wxFileName instead of wxString?


extern void initPaths(wxString path, wxString exe);

extern wxString getExecutable();
extern wxString getRootDir();

extern wxString getImagePath(wxString img);
extern wxString getSoundPath(wxString snd);

#endif /* MYABCS_PATHS_H */
