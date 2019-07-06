
/* Copyright © 2019 Jordan Irwin (AntumDeluge) <antumdeluge@gmail.com>
 *
 * This file is part of the MyABCs project & is distributed under the
 * terms of the MIT/X11 license. See: LICENSE.txt
 */

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

ResourceObject ResourceList::getObject(unsigned int idx) {
	// FIXME: failsafe check for index out of range
	return objects.at(idx);
}

ResourceObject ResourceList::getObject(wxString alpha) {
	// FIXME: failsafe check for index out of range
	return getObject(getObjectIndex(alpha));
}

ResourceObject ResourceList::getNext(wxString alpha) {
	// FIXME: falsafe check for index out of range
	return getObject(getObjectIndex(alpha) + 1);
}

ResourceObject ResourceList::getPrev(wxString alpha) {
	// FIXME: falsafe check for index out of range
	return getObject(getObjectIndex(alpha) - 1);
}

void ResourceList::clear() {
	objects.clear();
}

void ResourceList::removeIndex(int idx) {
	objects.erase(objects.begin() + idx);
}
