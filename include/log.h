
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#ifndef MYABCS_LOG_H
#define MYABCS_LOG_H

#include <string>
#include <wx/log.h>
#include <wx/string.h>
#include <wx/window.h>


/** Log window class
 *
 */
class LogWindow: public wxLogWindow {
public:
	LogWindow(wxWindow* parent, wxString title);
	bool IsShown();
	void Show(const bool show);
	void Show() { return Show(true); }
};


extern void initLog(wxWindow* parent);

extern void logMessage(const wxLogLevel level, const wxString msg);

extern void logMessage(const wxString msg);

extern void logError(const wxString msg);

extern void logCurrentError();

extern void toggleLogWindow();

extern void setErrorCode(int err);

extern int getErrorCode();

extern void setErrorMsg(std::string msg);

extern void setErrorMsg(wxString msg);

extern std::string getErrorMsg();

extern void setError(int err, std::string msg);

#endif /* MYABCS_LOG_H */
