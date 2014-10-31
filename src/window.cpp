#include "window.h"

MainWindow::MainWindow(const wxString& title)
  : wxFrame(NULL, wxNewId(), title, wxDefaultPosition, wxDefaultSize,
          wxDEFAULT_FRAME_STYLE | wxMAXIMIZE) {
    
    wxImage img;
    
    Centre();
    
    bgpanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    bgpanel->SetBackgroundColour(wxColour(_("white")));
    bgpanel->SetFocus();
    
    svg = new wxSVGCtrl(bgpanel);
    svg->SetBackgroundColour(wxColour(_("white")));
    
    wxBoxSizer* m_hSizer = new wxBoxSizer(wxVERTICAL);
    m_hSizer->Add(svg, 1, wxEXPAND | wxALIGN_CENTER);
    
    bgpanel->SetAutoLayout(true);
    bgpanel->SetSizer(m_hSizer);
    bgpanel->Layout();
    
    wxBoxSizer* m_vSizer = new wxBoxSizer(wxVERTICAL);
    m_vSizer->Add(bgpanel, 1, wxEXPAND | wxALIGN_CENTER);
    
    SetAutoLayout(true);
    SetSizer(m_vSizer);
    Layout();
    
    setImage(_("data/logo/sdl.svg"));
    
}

void MainWindow::setImage(const wxString& image) {
    svg->Load(image);
    svg->SetFitToFrame();
    svg->Refresh();
    bgpanel->Refresh();
}
