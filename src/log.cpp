#include "log.h"

#include <iostream>
#include <wx/frame.h>
#include <wx/window.h>

using namespace std;


static LogWindow* abclog;


LogWindow::LogWindow(wxWindow* parent, wxString title) :
		wxLogWindow(parent, title, false) {}

bool LogWindow::IsShown() {
	return GetFrame()->IsShown();
}

void LogWindow::Show(const bool show) {
	wxWindow* frame = GetFrame();
	const bool wasShown = frame->IsShown();

	wxLogWindow::Show(show);

	if (!wasShown && frame->IsShown()) {

		wxWindow* mainFrame = frame->GetParent();
		if (!mainFrame->HasFocus()) {
			// give focus back to main window
			mainFrame->SetFocus();
		}
	}
}


void initLog(wxWindow* parent) {
	abclog = new LogWindow(parent, _T("MyABCs Debug Log"));
	wxLog::SetActiveTarget(abclog);
}

void logMessage(const wxLogLevel level, const wxString msg) {
	if (abclog->IsEnabled()) {
		wxLogGeneric(level, msg);
		cout << msg << endl;
	}
}

void logMessage(const wxString msg) {
	logMessage(wxLOG_Info, msg);
}

void logError(const wxString msg) {
	logMessage(wxLOG_Error, msg);
}

void toggleLogWindow() {
	abclog->Show(!abclog->IsShown());
}
