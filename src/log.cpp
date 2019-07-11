
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "log.h"

#include <iostream>
#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/window.h>

using namespace std;

static wxLog* logger;
static bool initialized = false;

static int err_code = 0;
static string err_msg = "";


/** private: only used in this source */
void resetError() {
	err_code = 0;
	err_msg = "";
}


LogWindow::LogWindow(wxWindow* parent, wxString title) :
		wxLogWindow(parent, title, false) {

	wxFrame* frame = GetFrame();
	// prevent log window showing in the tas bar
	frame->SetWindowStyle(frame->GetWindowStyle()|~wxFRAME_NO_TASKBAR);
}

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


void initLog() {
	if (!initialized) {
		logger = new wxLogStream(&cout);
		wxLog::SetActiveTarget(logger);
		initialized = true;
	}
}

void logMessage(const wxLogLevel level, const wxString msg) {
	wxLogGeneric(level, msg);
	if (level == wxLOG_Error) {
		wxMessageBox(msg, "Error", wxICON_ERROR);
	}
}

void logMessage(const wxString msg) {
	logMessage(wxLOG_Info, msg);
}

void logError(const wxString msg) {
	logMessage(wxLOG_Error, msg);
}

/** prints stored error info in log */
void logCurrentError() {
	string msg = "";

	if (err_msg != "") {
		msg = err_msg;
		if (err_code != 0) {
			msg += " (error code: " + to_string(err_code) + ")";
		}
	} else {
		if (err_code != 0) {
			msg = "Error code: " + to_string(err_code);
		}
	}

	if (err_msg == "") {
		logError("Unknown error");
	} else {
		logError(wxString(msg));
	}

	resetError();
}

void disableLogging() {
	wxLog::EnableLogging(false);
}

void setErrorCode(int err) { err_code = err; }

int getErrorCode() { return err_code; }

void setErrorMsg(string msg) { err_msg = msg; }

void setErrorMsg(wxString msg) { setErrorMsg(string(msg)); }

string getErrorMsg() { return err_msg; }

void setError(int err, string msg) {
	setErrorCode(err);
	setErrorMsg(msg);
}
