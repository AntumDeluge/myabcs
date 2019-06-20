#include <iostream>

#include "log.h"


static wxLogWindow* abclog;


void initLog(wxWindow* parent) {
	abclog = new wxLogWindow(parent, _T("MyABC's Debug Log"), false);
	wxLog::SetActiveTarget(abclog);
}

void logMessage(const wxLogLevel level, const wxString msg) {
	if (abclog->IsEnabled()) {
		abclog->Show();
		wxLogGeneric(level, msg);
		std::cout << msg << std::endl;
	}
}

void logMessage(const wxString msg) {
	logMessage(wxLOG_Info, msg);
}

void logError(const wxString msg) {
	logMessage(wxLOG_Error, msg);
}
