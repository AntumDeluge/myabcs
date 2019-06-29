#include "log.h"
#include "resourcelist.h"


void ResourceList::set(ResourceList rl) {
	// delete previous objects
	clear();

	for (ResourceObject ro : rl.getObjects()) {
		objects.push_back(ro);
	}
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

int ResourceList::getObjectIndex(wxString alpha) {
	alpha = alpha.Lower();

	for (int idx = 0; objects.size(); idx++) {
		wxString label = objects.at(idx).getLabel().Lower();
		if (label.StartsWith(alpha)) {
			return idx;
		}
	}

	return -1;
}

ResourceObject ResourceList::getObject(wxString alpha) {
	const int idx = getObjectIndex(alpha);

	// FIXME: how to return NULL or negative value to signify failure???
	/*
	if (idx < 0) {
		return;
	}
	*/

	return objects.at(idx);
}

void ResourceList::clear() {
	objects.clear();
}

void ResourceList::removeIndex(int idx) {
	objects.erase(objects.begin() + idx);
}
