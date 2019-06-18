#ifndef MYABCS_LOG_H
#define MYABCS_LOG_H

#include <wx/log.h>

// instantiated in main wxApp
static wxLogWindow* abclog;

static void initLog(wxWindow* parent) {
	abclog = new wxLogWindow(parent, _T("MyABC's Debug Log"), false);
	wxLog::SetActiveTarget(abclog);
}

static wxLogWindow* getLog() {
	return abclog;
}

static void logMessage(const wxLogLevel level, const wxString msg) {
	if (abclog->IsEnabled()) {
		abclog->Show();
		wxLogGeneric(level, msg);
	}
}

static void logMessage(const wxString msg) {
	logMessage(wxLOG_Info, msg);
}

#endif /* MYABCS_LOG_H */
