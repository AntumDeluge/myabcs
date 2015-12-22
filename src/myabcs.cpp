#include "myabcs.h"
#include "window.h"

wxIMPLEMENT_APP(MyABCs);

bool MyABCs::OnInit() {
    MainWindow* frame = new MainWindow(_("MyABCs"));
    frame->Show(true);
    SetTopWindow(frame);
    
    return true;
}
