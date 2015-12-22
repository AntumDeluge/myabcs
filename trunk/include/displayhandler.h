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

#ifndef MYABCS_DISPLAYHANDLER_H
#define MYABCS_DISPLAYHANDLER_H

#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/window.h>

#include "imagehandler.h"

class DisplayHandler : public wxPanel {
public:
    DisplayHandler(wxWindow* parent);
    const wxString* getCategory() { return category; }
    void reload();
    void resizeImage(const wxSize square) { this->imagectrl->resizeImage(square); }
    void setCategory(const wxString category);
    void setDisplay(const wxString name);
private:
    wxStaticText* alphactrl;
    ImageHandler* imagectrl;
    wxStaticText* textctrl;
    wxString* category;
};

#endif /* MYABCS_DISPLAYHANDLER_H */
