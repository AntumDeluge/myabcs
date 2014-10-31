#ifndef MYABCS_WINDOW
#define MYABCS_WINDOW

#include <wx/wx.h>
#include <wxSVG/svg.h>
#include <wxSVG/svgctrl.h>

class MainWindow : public wxFrame {
public:
    MainWindow(const wxString& title);
    void setImage(const wxString& image);
private:
    wxPanel* bgpanel;
    wxSVGCtrl* svg;
    wxStaticBitmap* imgdisplay;
};

#endif /* MYABCS_WINDOW */
