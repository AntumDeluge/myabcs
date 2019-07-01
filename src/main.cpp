#include "env.h"
#include "main.h"
#include "abc.h"
#include "log.h"
#include "sound.h"

#include <string>
#include <wx/stdpaths.h>


bool App::OnInit() {
	// initialize environment directories
	initPaths(std::string(wxPathOnly(wxStandardPaths::Get().GetExecutablePath()).mb_str()));

	// DEBUG:
	logMessage(wxString::Format("Root dir: %s", getRootDir()));

	// TODO: allow initializing sounds without dependency on wxApp
	// initialize SDL audio
	initSound();

	// initialize image support
	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxICOHandler);
	wxImage::AddHandler(new wxXPMHandler);

	MainWindow* frame = new MainWindow(_T(""));
	frame->Show(true);
	SetTopWindow(frame);

	// FIXME: logging is not initialized for MainWindow constructor
	initLog(frame);

	return true;
}


int main(int argc, char** argv) {
	App* app = new App();
	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	return 0;
}
