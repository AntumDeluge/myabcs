
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
 *
 * TODO: Use std::array or other type immutable size in place of std::vector
 */
class ResourceList {
public:
	void set(ResourceList rl);
	void add(ResourceObject ro) { objects.push_back(ro); }
	bool remove(wxString alpha);
	int getObjectIndex(wxString alpha);
	ResourceObject getObject(unsigned int idx);
	ResourceObject getObject(wxString alpha);
	ResourceObject getNext(wxString alpha);
	std::vector<ResourceObject> getObjects() { return objects; }
	int count() { return objects.size(); }
	void clear();
private:
	std::vector<ResourceObject> objects;

	void removeIndex(int idx);
};


#endif /* MYABCS_RESOURCELIST_H */
