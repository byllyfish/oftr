#include "ofp/oxmlist.h"

namespace ofp { // <namespace ofp>


#if 0
void ofp::OXMList::add(const void *data, size_t len) 
{
	const UInt8 *p = static_cast<const UInt8*>(data);
	buf_.insert(buf_.end(), p, p + len);
}
#endif //0


void OXMList::insert(OXMIterator pos, const void *data, size_t len)
{
	buf_.insert(pos.data(), data, len);
	#if 0
	const UInt8 *p = static_cast<const UInt8*>(data);
	auto i = buf_.begin() + (pos.data() - &buf_[0]);
	buf_.insert(i, p, p + len);
	#endif //0
}


OXMList::OXMList(const OXMRange &range)
{
	buf_.add(range.data(), range.size());
}


void OXMList::add(OXMType type, const void *data, size_t len)
{
	assert(type.length() == len);
	
	buf_.add(&type, sizeof(type));
	buf_.add(data, len);
}


void OXMList::add(OXMType type, const void *data, const void *mask, size_t len)
{
	assert(type.length() == 2*len);

	buf_.add(&type, sizeof(type));
	buf_.add(data, len);					// FIXME - apply mask here.
	buf_.add(mask, len);
}



OXMIterator OXMList::replace(OXMIterator pos, OXMIterator end, OXMType type, const void *data, size_t len)
{
	assert(type.length() == len);
	assert(end.data() > pos.data());

	ptrdiff_t idx = buf_.offset(pos.data());
	size_t newlen = sizeof(OXMType) + len;
	buf_.replaceUninitialized(pos.data(), end.data(), newlen);

	const UInt8 *tptr = reinterpret_cast<const UInt8 *>(&type);
	std::copy(tptr, tptr + sizeof(type), buf_.mutableData() + idx);
	const UInt8 *dptr = static_cast<const UInt8 *>(data);
	std::copy(dptr, dptr + len, buf_.mutableData() + idx + sizeof(type));

	OXMIterator rest{buf_.data() + idx + newlen};
	assert(rest <= this->end());
	assert(this->begin() <= rest);

	return rest;

#if 0
	size_t origlen = Unsigned_cast(end.data() - pos.data());
	size_t newlen = sizeof(OXMType) + len;

	auto ipos = (pos.data() - &buf_[0]);
	auto iend = (end.data() - &buf_[0]);

	if (newlen > origlen) {
		buf_.insert(buf_.begin() + iend, newlen - origlen, 0);
	} else if (newlen < origlen) {
		buf_.erase(buf_.begin() + ipos, buf_.begin() + Unsigned_cast(ipos) + (origlen - newlen));
	}

	const UInt8 *tptr = reinterpret_cast<const UInt8 *>(&type);
	std::copy(tptr, tptr + sizeof(type), buf_.begin() + ipos);
	const UInt8 *dptr = static_cast<const UInt8 *>(data);
	std::copy(dptr, dptr + len, buf_.begin() + ipos + sizeof(type));

	// Return an iterator to the rest of the entries.
	
	OXMIterator rest{&buf_[Unsigned_cast(ipos) + sizeof(type) + len]};
	assert(rest <= this->end());
	assert(this->begin() <= rest);

	return rest;
#endif //0
}


#if 0
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
#endif


} // </namespace ofp>

