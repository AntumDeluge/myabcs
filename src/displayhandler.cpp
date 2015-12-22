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

#include <wx/sizer.h>

#include "displayhandler.h"

DisplayHandler::DisplayHandler(wxWindow* parent) :
        wxPanel(parent) {
    this->SetFocus();
    this->SetBackgroundColour(wxT("white"));

    alphactrl = new wxStaticText(this, wxID_ANY, wxEmptyString);
    imagectrl = new ImageHandler(this);
    textctrl = new wxStaticText(this, wxID_ANY, wxEmptyString);

    wxBoxSizer* alphabox = new wxBoxSizer(wxHORIZONTAL);
    alphabox->Add(alphactrl, 1, wxEXPAND | wxALIGN_CENTER);

    wxBoxSizer* imagebox = new wxBoxSizer(wxHORIZONTAL);
    imagebox->Add(imagectrl->getImageObject(), 1, wxEXPAND | wxALIGN_CENTER);

    wxBoxSizer* textbox = new wxBoxSizer(wxHORIZONTAL);
    textbox->Add(textctrl, 1, wxEXPAND | wxALIGN_CENTER);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(alphabox, 0, wxEXPAND | wxALIGN_CENTER);
    vbox->Add(imagebox, 1, wxEXPAND | wxALIGN_CENTER);
    vbox->Add(textbox, 0, wxEXPAND | wxALIGN_CENTER);

    this->SetAutoLayout(true);
    this->SetSizer(vbox);
    this->Layout();
}

void DisplayHandler::reload() {
    this->Refresh();
}

void DisplayHandler::setCategory(wxString category) {
    this->category = &category;
}

void DisplayHandler::setDisplay(const wxString name) {
    // FIXME: Need failsafe to ensure "item" is not empty string
    textctrl->SetLabel(name.Upper());
    // FIXME: SVG image and size should be set automatically
    imagectrl->setSVG(wxT("data/image/main/airplane.svg"), wxSize(256, 256));
    alphactrl->SetLabel(name.Upper().GetChar(0));
}
