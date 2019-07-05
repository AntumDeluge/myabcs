#ifndef MYABCS_PATHS_H
#define MYABCS_PATHS_H

#include <wx/string.h>

// XXX: any benefit to using wxFileName instead of wxString?


extern void initPaths(wxString path, wxString exe);

extern wxString getExecutable();
extern wxString getRootDir();

extern wxString getImageFile(wxString img);
extern wxString getSoundFile(wxString snd);

#endif /* MYABCS_PATHS_H */
