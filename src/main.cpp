#include "main.h"
#include "abc.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    MainWindow *frame = new MainWindow(_T(""));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}
