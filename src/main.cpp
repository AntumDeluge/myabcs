
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

#include "fonts.h"
#include "id.h"
#include "main.h"
#include "abc.h"
#include "log.h"
#include "paths.h"
#include "sound.h"

#include <string>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/string.h>


bool App::OnInit() {
	// FIXME: logger needs initialized before everything else

	// initialize environment directories
	wxString full_path = wxStandardPaths::Get().GetExecutablePath();
	initPaths(std::string(wxPathOnly(full_path).mb_str()), std::string(wxFileName::FileName(full_path).GetFullName().mb_str()));

	// DEBUG:
	logMessage(wxString::Format("Set executable: %s", getExecutable()));
	logMessage(wxString::Format("Set root dir: %s", getRootDir()));

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
	frame->setCategory(ID_ABC);

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
