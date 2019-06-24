#ifndef MYABCS_LOG_H
#define MYABCS_LOG_H

#include <wx/log.h>


/** Log window class
 *
 */
class LogWindow: public wxLogWindow {
public:
	LogWindow(wxWindow* parent, wxString title, bool show=true);
	bool IsShown();
	void Show(const bool show);
	void Show() { return Show(true); }
};


extern void initLog(wxWindow* parent);

extern void logMessage(const wxLogLevel level, const wxString msg);

extern void logMessage(const wxString msg);

extern void logError(const wxString msg);

extern void toggleLogWindow();

#endif /* MYABCS_LOG_H */
