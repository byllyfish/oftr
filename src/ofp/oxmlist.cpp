#include "ofp/oxmlist.h"
#include <cassert>


inline void ofp::OXMList::add(const void *data, size_t len) 
{
	const UInt8 *p = static_cast<const UInt8*>(data);
	buf_.insert(buf_.end(), p, p + len);
}


ofp::OXMList::OXMList(const OXMRange &range)
{
	add(range.data(), range.size());
}


void ofp::OXMList::add(OXMType type, const void *data, size_t len)
{
	assert(type.length() == len);
	
	add(&type, sizeof(type));
	add(data, len);
}


void ofp::OXMList::add(OXMType type, const void *data, const void *mask, size_t len)
{
	assert(type.length() == 2*len);
	
	add(&type, sizeof(type));
	add(data, len);
	add(mask, len);
}
