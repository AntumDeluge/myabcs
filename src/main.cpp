#include "main.h"
#include "abc.h"


bool App::OnInit()
{
    MainWindow* frame = new MainWindow(_T(""));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}


int main(int argc, char** argv) {
	App* app = new App();
	wxApp::SetInstance(app);
	wxEntry(argc, argv);

	return 0;
}
