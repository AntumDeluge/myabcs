#include <array>
#include <iostream>
#include <wx/string.h>

using namespace std;


int main(int argc, char** argv) {
	array<wxString, 2> items { "item1", "item2", };

	for (wxString i : items) {
		cout << "Item: " << i.c_str() << "\n";
	}

	return 0;
}
