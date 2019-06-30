#include <iostream>
#include <wx/regex.h>
#include <wx/string.h>

using namespace std;


const static wxRegEx alpha(_T("^[A-Za-z]+$"));


bool isAlphaOnly(wxString s) {
	cout << "Checking string \"" << s.c_str() << "\":\t";

	return alpha.Matches(s);
}

string toString(bool b) {
	if (b) return "true";
	else return "false";
}


int main(int argc, char** argv) {
	wxString s1 = "HellowxWidets";
	wxString s2 = "Hello wxWidgets";
	wxString s3 = "HellowxWidgets!";
	wxString s4 = "Hello8wxWidgets";

	cout << "\nAlpha only:\n";
	cout << toString(isAlphaOnly(s1)) << " (should be 'true')\n";
	cout << toString(isAlphaOnly(s2)) << " (should be 'false')\n";
	cout << toString(isAlphaOnly(s3)) << " (should be 'false')\n";
	cout << toString(isAlphaOnly(s4)) << " (should be 'false')\n";

	return 0;
}
