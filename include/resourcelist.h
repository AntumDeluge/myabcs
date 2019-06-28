
/* Interface for caching resources */

#ifndef MYABCS_RESOURCE_LIST_H
#define MYABCS_RESOURCE_LIST_H

#include "resourceobject.h"

#include <vector>


/** Class representing a list of resources loaded into memory
 *
 * Resources are identified by the first letter in the wxString name.
 * The list should not contain more than one object representing a
 * single letter.
 */
class ResourceList {
public:
	~ResourceList();
	void add(ResourceObject* ro) { objects.push_back(ro); }
	bool remove(wxString alpha);
	int getObjectIndex(wxString alpha);
	int count() { return objects.size(); }
	ResourceObject* getObject(wxString alpha);
	void clear();
private:
	std::vector<ResourceObject*> objects;

	void removeIndex(int idx);
};


#endif /* MYABCS_RESOURCELIST_H */
