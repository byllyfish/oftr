#include "ofp/oxmrange.h"
#include "ofp/log.h"

#if 0
/**
 *  Check sequence of OXM records to make sure they have the correct format:
 *
 *  For all OXMs:
 *  - Check that prerequisites exist.
 *  - Check for fields with improper masks.
 *  - Check that no two consecutive fields have identical types.
 */
bool ofp::OXMRange::validate() const
{
	// TODO: use a bitset to keep track of OFB fields. (Note: this doesn't
	// generalize to custom fields though, unless they use OFB fields as
	// prereqs.)
	// std::bitset<128> fields;

	for (auto item : *this) {
		OXMType type = item.type();
		auto info = type.lookupInfo();
		assert(info);

		if (!info) {
			log::info("Unknown oxm type ", type);
			continue;
		}

		if (type.hasMask() && !info->isMaskSupported) {
			log::info("Invalid mask for ", type);
			return false;

		} else {
			OXMRange range{ begin(), item.position() };
			if (!checkPrerequisites(range, info->prerequisites)) {
				log::info("Missing prerequisite for ", type);
				return false;
			}
		}
	}

	return true;
}


/**
 *  Check sequence of OXM records to make sure that prerequisites are met.
 *  We only need to search until `pos`; that's the position of the item we're
 *  validating. Use obvious O(n^2) algorithm. Return true if there are no
 *  prerequisites (list is null).
 */
bool ofp::OXMRange::checkPrerequisites(const OXMRange &range, const OXMRange *prereqs)
{
	if (!prereqs)
		return true;

	OXMIterator preq = prereqs->begin();
	OXMIterator preqEnd = prereqs->end();

	while (preq != preqEnd) {
		OXMType preqType = preq->type();
		if (preqType.hasMask()) {
			if (!findPrerequisiteMask(range, preqType, &preq, preqEnd)) 
				return false;

		} else {
			if (!findPrerequisiteNoMask(range, preqType, &preq, preqEnd))
				return false;
		}
	}

	return true;
}


/**
 *  Search for specified preq in the given range and advance the preq iterator. 
 *  Note that a preq may be encoded using two iterator positions; in that case
 *  we advance the iterator twice.
 */
bool ofp::OXMRange::findPrerequisiteNoMask(const OXMRange &range, OXMType preqType, OXMIterator *preq, OXMIterator preqEnd) 
{
	for (auto item : range) {
		OXMType type = item.type();
		if (type == preqType) {
			// When we find a matching type, check the value against the prereq.
			if (equalValues(type, preq->position_, item.position().position_)) {
				++(*preq);
				return true;
			}

			// Value didn't match preq. C
		}
	}

	return false;
}


bool ofp::OXMRange::equalValues(OXMType type, const UInt8 *lhs, const UInt8 *rhs)
{
	return std::memcmp(lhs + sizeof(OXMType), rhs + sizeof(OXMType), type.length()) == 0;
}
#endif

