/* -- MIT LICENSE --

 Copyright (c) 2015 Jordan Irwin

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*/

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
