#include "env.h"
#include "fonts.h"
#include "main.h"
#include "abc.h"
#include "log.h"
#include "sound.h"

#include <string>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/string.h>


bool App::OnInit() {
	// initialize environment directories
	wxString full_path = wxStandardPaths::Get().GetExecutablePath();
	initPaths(std::string(wxPathOnly(full_path).mb_str()), std::string(wxFileName::FileName(full_path).GetFullName().mb_str()));

	// DEBUG:
	logMessage(wxString::Format("Root dir: %s", getRootDir()));
	logMessage(wxString::Format("Executable: %s", executable));

	// initialize fonts
	initFonts();

	// TODO: allow initializing sounds without dependency on wxApp
	// initialize SDL audio
	initSound();

	// initialize image support
	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxICOHandler);
	wxImage::AddHandler(new wxXPMHandler);

	MainWindow* frame = new MainWindow();
	frame->Show(true);
	SetTopWindow(frame);

	// FIXME: logging is not initialized for MainWindow constructor
	initLog(frame);

	// start in "main" category
	frame->LoadCategory(_T("main"));

	return true;
}

int App::OnExit() {
	cleanupFonts();
	// FIXME: proper return code?
	return 0;
}


int main(int argc, char** argv) {
	App* app = new App();
	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	// cleanup
	delete soundPlayer;

	return 0;
}
