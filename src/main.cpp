#include "main.h"
#include "abc.h"
#include "log.h"
#include "sound.h"


bool App::OnInit()
{
	// initialize SDL audio
	initializeSounds();

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
