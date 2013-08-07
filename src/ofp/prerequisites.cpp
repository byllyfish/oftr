#include "ofp/prerequisites.h"
#include "ofp/oxmlist.h"
#include "ofp/log.h"
#include "ofp/oxmtypeset.h"

// Use an OXMType that can't possibly exist to signal placeholders
// for prerequisites. We assume an OXMType with the mask bit set, but
// a zero length will not legally appear in the protocol.

constexpr const ofp::OXMType kMaskedPrereqSignal = ofp::OXMType(0x7FF1, 0, 0).withMask();
constexpr const ofp::OXMType kValuePrereqSignal = ofp::OXMType(0x7FF2, 0, 0).withMask();


void ofp::Prerequisites::insertAll(OXMList *list, const OXMRange *preqs)
{
	if (preqs != nullptr) {
		Prerequisites p{preqs};
		p.insert(list);
	}
}


void ofp::Prerequisites::insert(OXMList *list) const
{
	assert(list != nullptr);

	// Insert prerequisites into the given list. We take advantage of the fact 
	// that the prerequisites are already topologically sorted.
	// FIXME: Make sure insert can't loop indefinitely.
	
	OXMIterator preq = preqs_->begin();
	OXMIterator preqEnd = preqs_->end();

	while (preq != preqEnd) {
		OXMType preqType = preq.type();

		if (preqType.hasMask()) {
			insertPreqMasked(preqType, preq, list);
		} else {
			insertPreqValue(preqType, preq, list);
		}

		advancePreq(&preq);
	}
}


void ofp::Prerequisites::insertPreqMasked(OXMType preqTypeMasked, OXMIterator preq, OXMList *list) const
{	
	if (!checkPreqMasked(preqTypeMasked, preq, list->begin(), list->end())) {
		// We add the masked prereq with a signal.
		list->addSignal(kMaskedPrereqSignal);
		list->add(preq);
	}
}

void ofp::Prerequisites::insertPreqValue(OXMType preqType, OXMIterator preq, OXMList *list) const
{
	if (!checkPreqValue(preqType, preq, list->begin(), list->end())) {
		list->add(preq);
	}
}

bool ofp::Prerequisites::checkAll(const OXMRange &oxm)
{
	// For each OXM entry, we will check if its prerequisites are met.
	// Complexity: If there are n items and each item has m prerequisites, this
	// algorithm is O(n*m).
	// 
	// FIXME: Make sure this check can't loop indefinitely.
	// TODO: Improve algorithm and/or data structures.

	for (auto item : oxm) {
		auto type = item.type();

		// Check for a type of length 0, but has the mask bit set. This is
		// treated as an illegal combination.

		if (type.isIllegal()) {
			log::info("Illegal oxm type ", type);
			goto FAILURE;
		}
		
		// Look up information record for the oxm type.
		auto typeInfo = type.lookupInfo();
		if (typeInfo == nullptr) {
			// Unrecognized oxm type. No way to check prerequisites. Skip it.
			log::info("Unknown oxm type ", type);
			continue;
		}

		assert(typeInfo != nullptr);

		if (type.hasMask()) {
			// The oxm type has a mask. Check if it's allowed to have one.
			if (!typeInfo->isMaskSupported) {
				log::info("Invalid mask for ", type);
				goto FAILURE;
			}
		}

		assert(!type.hasMask() || typeInfo->isMaskSupported);

		// Check if oxm type has prerequisites, and if so, check them all.
		if (typeInfo->prerequisites != nullptr) {
			Prerequisites preqs{typeInfo->prerequisites};
			if (!preqs.check(oxm.begin(), item.position())) {
				log::info("Prerequisite check failed for ", type);
				goto FAILURE;
			}
		}
	}

	return true;

FAILURE:
	log::info("Prerequiste check failed for ", oxm);
	return false;
}


bool ofp::Prerequisites::check(OXMIterator begin, OXMIterator end) const
{
	// Check this oxm range against our sequence of prerequisites.

	OXMIterator preq = preqs_->begin();
	OXMIterator preqEnd = preqs_->end();

	while (preq != preqEnd) {
		OXMType preqType = preq.type();

		if (preqType.hasMask()) {
			if (checkPreqMasked(preqType, preq, begin, end))
				return true;
			
		} else {
			if (checkPreqValue(preqType, preq, begin, end))
				return true;
		}

		advancePreq(&preq);
	}

	return false;
}


bool ofp::Prerequisites::checkPreqMasked(OXMType preqTypeMasked, OXMIterator preq, OXMIterator begin, OXMIterator end) const
{
	assert(preqTypeMasked.hasMask());

	OXMType preqType = preqTypeMasked.withoutMask();
	size_t valueLength = preqType.length();

	// Search for any values that match the preq. If we find one, return true.
	OXMIterator pos = begin;
	while (pos != end) {
		OXMType posType = pos.type();
		if (posType == preqType) {
			// Match `pos` value against prerequisite's value & mask.
			if (matchValueWithMask(valueLength, pos, preq)) {
				return true;
			}
			
		} else if (posType == preqTypeMasked) {
			// Match `pos` value & mask against prerequisite's value & mask.
			if (matchMaskWithMask(valueLength, pos, preq)) {
				return true;
			}
		} else {
			// Type doesn't match preq. Keep going.
		}
		++pos;
	}

	return false;
}


bool ofp::Prerequisites::checkPreqValue(OXMType preqType, OXMIterator preq, OXMIterator begin, OXMIterator end) const
{
	assert(!preqType.hasMask());

	OXMType preqTypeMasked = preqType.withMask();
	size_t valueLength = preqType.length();

	// Search for any values that match the preq. If we find one, return true.
	OXMIterator pos = begin;
	while (pos != end) {
		OXMType posType = pos.type();
		if (posType == preqType) {
			// Match `pos` value against prerequisite's value.
			if (matchValueWithValue(valueLength, pos, preq)) {
				return true;
			}
		} else if (posType == preqTypeMasked) {
			// Match `pos` value & mask against prerequisite's value.
			if (matchMaskWithValue(valueLength, pos, preq)) {
				return true;
			}
		} else {
			// Type doesn't match preq. Keep going.
		}
	}

	return false;
}


bool ofp::Prerequisites::matchValueWithMask(size_t length, OXMIterator pos, OXMIterator preq)
{
	assert(length == pos.type().length());

	return matchValueWithMask(length, pos.data() + sizeof(OXMType), preq);

	#if 0
	assert(2 * length == preq.type().length());

	// Return true if the value matches the value & mask in the preq.
	
	const UInt8 *value = pos.data() + sizeof(OXMType);
	const UInt8 *preqValue = preq.data() + sizeof(OXMType);
	const UInt8 *preqMask = preqValue + length;
	
	for (size_t i = 0; i < length; ++i) {
		if ((*value++ & *preqMask++) != *preqValue++)
			return false;
	}

	return true;
	#endif
}


bool ofp::Prerequisites::matchValueWithMask(size_t length, const void *data, OXMIterator preq)
{
	assert(2 * length == preq.type().length());

	// Return true if the value matches the value & mask in the preq.
	
	const UInt8 *value = static_cast<const UInt8 *>(data);
	const UInt8 *preqValue = preq.data() + sizeof(OXMType);
	const UInt8 *preqMask = preqValue + length;
	
	for (size_t i = 0; i < length; ++i) {
		if ((*value++ & *preqMask++) != *preqValue++)
			return false;
	}

	return true;	
}


bool ofp::Prerequisites::matchMaskWithMask(size_t length, OXMIterator pos, OXMIterator preq)
{
	assert(2 * length == pos.type().length());
	assert(2 * length == preq.type().length());

	// Return true if masked value fits inside preq's mask.
	
	const UInt8 *value = pos.data() + sizeof(OXMType);
	const UInt8 *mask = value + length;
	const UInt8 *preqValue = preq.data() + sizeof(OXMType);
	const UInt8 *preqMask = preqValue + length;

	for (size_t i = 0; i < length; ++i) {
		UInt8 maskedValue = (*value++ & *mask++);
		if ((maskedValue & *preqMask++) != *preqValue++) {
			return false;
		}
	}

	return true;
}


bool ofp::Prerequisites::matchValueWithValue(size_t length, OXMIterator pos, OXMIterator preq)
{
	assert(length == pos.type().length());
	assert(length == preq.type().length());

	// Return true if values are identical.

	const UInt8 *value = pos.data() + sizeof(OXMType);
	const UInt8 *preqValue = preq.data() + sizeof(OXMType);
	
	for (size_t i = 0; i < length; ++i) {
		if (*value++ != *preqValue++) {
			return false;
		}
	}

	return true;
}


bool ofp::Prerequisites::matchMaskWithValue(size_t length, OXMIterator pos, OXMIterator preq)
{
	assert(2*length == pos.type().length());
	assert(length == preq.type().length());

	// Return true if pos mask is all ones and values are identical.

	const UInt8 *value = pos.data() + sizeof(OXMType);
	const UInt8 *mask = value + length;
	const UInt8 *preqValue = preq.data() + sizeof(OXMType);

	for (size_t i = 0; i < length; ++i) {
		if ((*mask++ != 0xFF) || (*value++ != *preqValue++)) {
			return false;
		}
	}

	return true;
}


bool ofp::Prerequisites::substitute(OXMList *list, OXMType type, const void *value, size_t len)
{
	// Substitute value for prereq placeholder if we find one that matches the type.
	assert(!type.hasMask());

	OXMIterator iter = list->begin();
	OXMIterator iterEnd = list->end();

	while (iter != iterEnd) {
		auto escType = iter.type();
		if (escType != kMaskedPrereqSignal) {
			++iter;
			continue;
		}

		OXMIterator start = iter;
		++iter;
		if (iter == iterEnd) {
			break;
		}

		auto preqType = iter.type();
		assert(preqType.hasMask());

		if (preqType.withoutMask() == type && matchValueWithMask(len, value, iter)) {
			++iter;
			// Perform substitution into OXMList. 
			// N.B. This may invalidate iterators.
			list->replace(start, iter, type, value, len);
			return true;
		}
		++iter;
	}

	return false;
}


bool ofp::Prerequisites::substitute(OXMList *list, OXMType type, const void *value, const void *mask, size_t len)
{
	return false;
}


/**
 *  Check for the presence of duplicate fields in the oxm list.
 */
bool ofp::Prerequisites::duplicateFieldsDetected(const OXMRange &oxm)
{
	OXMTypeSet typeSet;

	for (auto item : oxm) {
		OXMType type = item.type();
		if (!typeSet.add(type)) {
			log::info("Duplicate field detected: ", type);
			return true;
		}
	}

	return false;
}
