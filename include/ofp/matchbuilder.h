#ifndef OFP_MATCHBUILDER_H
#define OFP_MATCHBUILDER_H

#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include "ofp/prerequisites.h"
#include "ofp/oxmconstants.h"

namespace ofp { // <namespace ofp>

class MatchBuilder {
public:

	MatchBuilder() = default;

	const UInt8 *data() const { return list_.data(); }
	size_t size() const { return list_.size(); }

	template <class ValueType>
	void add(ValueType value) {
		Prerequisites::insertAll(&list_, ValueType::prerequisites());
		if (!Prerequisites::substitute(&list_, ValueType::type(), &value, sizeof(value))) {
			list_.add(value);
		}
	}
	
	template <class ValueType>
	void add(ValueType value, ValueType mask) {
		static_assert(ValueType::isMaskSupported(), "mask not supported.");
		Prerequisites::insertAll(&list_, ValueType::prerequisites());
		if (!Prerequisites::substitute(&list_, value, mask)) {
			list_.add(value, mask);
		}
	}

	bool validate() const {
		return Prerequisites::checkAll(list_.toRange()) &&
				!Prerequisites::duplicateFieldsDetected(list_.toRange());
	}

	OXMRange toRange() const { return list_.toRange(); }

private:

	OXMList list_;
};

} // </namespace ofp>

#endif // OFP_MATCHBUILDER_H
