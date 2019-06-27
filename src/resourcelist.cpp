#include "log.h"
#include "resourcelist.h"

/*
#include <fstream>
#include <wx/filefn.h>
#include <wx/wfstream.h>
using namespace std;
*/


ResourceList::~ResourceList() {
	for (unsigned int idx = 0; idx < objects.size(); idx++) {
		delete objects.at(idx);
	}

	objects.clear();
}

/** Removes first instance found */
bool ResourceList::remove(wxString alpha) {
	const int idx = getObjectIndex(alpha);

	if (idx < 0) {
		return false;
	}

	removeIndex(idx);

	return getObjectIndex(alpha) < 0;
}

/** Checks if object identified by alpha exists */
/*
bool ResourceList::containsAlpha(wxString alpha) {
	alpha = alpha.Lower();

	for

	return false;
}
*/

int ResourceList::getObjectIndex(wxString alpha) {
	alpha = alpha.Lower();

	for (int idx = 0; objects.size(); idx++) {
		wxString label = objects.at(idx)->getLabel().Lower();
		if (label.StartsWith(alpha)) {
			return idx;
		}
	}

	return -1;
}

ResourceObject* ResourceList::getObject(wxString alpha) {
	const int idx = getObjectIndex(alpha);

	// FIXME: how to return NULL or negative value to signify failure???
	/*
	if (idx < 0) {
		return;
	}
	*/

	return objects.at(idx);
}

void ResourceList::removeIndex(int idx) {
	delete objects.at(idx);
	objects.erase(objects.begin() + idx);
}
