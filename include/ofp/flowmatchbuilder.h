#ifndef OFP_FLOWMATCHBUILDER_H
#define OFP_FLOWMATCHBUILDER_H

#include "ofp/oxmlist.h"

namespace ofp { // <namespace ofp>

class FlowMatchBuilder {
public:

	const UInt8 *data() const { return list_.data(); }
	size_t size() const { return list_.size(); }
	
	template <class ValueType>
	void add(ValueType value) {
		list_.insertPrerequisites(ValueType::prerequisites());
		list_.add(value);
	}
	
	template <class ValueType>
	void add(ValueType value, ValueType mask) {
		static_assert(ValueType::isMaskSupported(), "mask not supported.");
		list_.insertPrerequisites(ValueType::prerequisites());
		list_.add(value, mask);
	}

private:
	OXMList list_;
};


std::ostream &operator<<(std::ostream &strm, const FlowMatchBuilder &builder)
{
	return strm << RawDataToHex(builder.data(), builder.size());
}


} // </namespace ofp>

#endif //OFP_FLOWMATCHBUILDER_H
