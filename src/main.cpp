#include "main.h"
#include "abc.h"
#include "log.h"
#include "sound.h"


bool App::OnInit()
{
    MainWindow* frame = new MainWindow(_T(""));
    frame->Show(true);
    SetTopWindow(frame);

    // FIXME: logging is not initialized for MainWindow constructor
    initLog(frame);

    // initialize SDL audio
    initializeSounds();

    return true;
}


int main(int argc, char** argv) {
	App* app = new App();
	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	return 0;
}
