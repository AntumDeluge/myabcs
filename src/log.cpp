#include <iostream>
#include <wx/frame.h>
#include <wx/window.h>

#include "log.h"


static LogWindow* abclog;


LogWindow::LogWindow(wxWindow* parent, wxString title, bool show=true) :
		wxLogWindow(parent, title, show) {
}

void LogWindow::Show(const bool show) {
	wxLogWindow::Show(show);

	wxWindow* mainFrame = GetFrame()->GetParent();
	if (show && !mainFrame->HasFocus()) {
		// give focus back to main window
		mainFrame->SetFocus();
	}
}


void initLog(wxWindow* parent) {
	abclog = new LogWindow(parent, _T("MyABCs Debug Log"), false);
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
