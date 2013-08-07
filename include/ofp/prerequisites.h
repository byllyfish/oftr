#ifndef OFP_PREREQUISITES_H
#define OFP_PREREQUISITES_H

#include "ofp/oxmtype.h"
#include "ofp/oxmrange.h"

namespace ofp { // <namespace ofp>

class OXMList;

class Prerequisites {
public:

	static void insertAll(OXMList *list, const OXMRange *preqs);
	static bool checkAll(const OXMRange &oxm);

	static bool substitute(OXMList *list, OXMType type, const void *value, size_t len);
	static bool substitute(OXMList *list, OXMType type, const void *value, const void *mask, size_t len);

	static bool duplicateFieldsDetected(const OXMRange &oxm);

private:
	const OXMRange *preqs_;

	explicit Prerequisites(const OXMRange *preqs) : preqs_{preqs} {
		assert(preqs != nullptr);
	}

	void insert(OXMList *list) const;
	bool check(OXMIterator begin, OXMIterator end) const;

	void insertPreqMasked(OXMType preqTypeMasked, OXMIterator preq, OXMList *list) const;
	void insertPreqValue(OXMType preqType, OXMIterator preq, OXMList *list) const;

	bool checkPreqMasked(OXMType preqTypeMasked, OXMIterator preq, OXMIterator begin, OXMIterator end) const;
	bool checkPreqValue(OXMType preqType, OXMIterator preq, OXMIterator begin, OXMIterator end) const;
	static bool matchValueWithMask(size_t length, OXMIterator pos, OXMIterator preq);
	static bool matchValueWithMask(size_t length, const void *data, OXMIterator preq);
	static bool matchMaskWithMask(size_t length, OXMIterator pos, OXMIterator preq);
	static bool matchValueWithValue(size_t length, OXMIterator pos, OXMIterator preq);
	static bool matchMaskWithValue(size_t length, OXMIterator pos, OXMIterator preq);

	static void advancePreq(OXMIterator *preq) { ++(*preq); }
};

} // </namespace ofp>

#endif // OFP_PREREQUISITES_H
