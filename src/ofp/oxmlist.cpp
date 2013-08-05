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
	add(data, len);					//TODO - apply mask here?
	add(mask, len);
}


void ofp::OXMList::insertPrerequisites(const OXMRange *values)
{
	// No prerequisites? Make a quick exit.
	if (values == nullptr)
		return;

	auto iter = begin();
	auto iterEnd = end();

	auto preqIter = values->begin();
	auto preqIterEnd = values->end();
	
	while (preqIter != preqIterEnd) {
		OXMType preqType = preqIter.type();
		OXMType iterType{};
	
		while (iter != iterEnd) {
			iterType = iter.type();
			if (iterType >= preqType)
				break;
			++iter;
		}
			
		if (iter == iterEnd) {
			// Case 1: iter == end
			// Insert [preqIter, preqIterEnd) at end of list.
			buf_.insert(buf_.end(), preqIter.data(), preqIterEnd.data());
			// We're done.
			return;
		} else if (iterType > preqType) {
			// Case 2: iter.type() > preq.type()
			// Insert [preqIter, preqIter+1) into list before iter.
			auto pos = buf_.begin() + (iter.data() - begin().data());
			buf_.insert(pos, preqIter.data(), preqIter.data() + sizeof(preqType) + preqType.length());
			// TODO may be able to extend preqIter+1...
		} else {
			// Case 3: iter == preq
			assert(iterType == preqType);
			// Ignore this prereq; it's already in the list.
		}
		
		++preqIter;
	}
}
