#include "myabcs.h"
#include "window.h"

IMPLEMENT_APP(MyABCs)

bool MyABCs::OnInit() {
    MainWindow* frame = new MainWindow(_("MyABCs"));
    frame->Show(true);
    SetTopWindow(frame);
    
    return true;
}
