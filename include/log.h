#ifndef MYABCS_LOG_H
#define MYABCS_LOG_H

#include <wx/log.h>


extern void initLog(wxWindow* parent);

extern void logMessage(const wxLogLevel level, const wxString msg);

extern void logMessage(const wxString msg);

#endif /* MYABCS_LOG_H */
