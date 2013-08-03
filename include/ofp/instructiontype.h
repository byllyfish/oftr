#ifndef OFP_INSTRUCTIONTYPE_H
#define OFP_INSTRUCTIONTYPE_H

#include "ofp/byteorder.h"

namespace ofp { // <namespace ofp>

class InstructionType {
public:

	enum : UInt16 {
		IT_GOTO_TABLE = 1,
		IT_WRITE_METADATA = 2,
		IT_WRITE_ACTIONS = 3,
		IT_APPLY_ACTIONS = 4,
		IT_CLEAR_ACTIONS = 5,
		IT_METER = 6,
		IT_EXPERIMENTER = 0xFFFF
	};

	constexpr explicit InstructionType(UInt16 type) : type_{type} {}

	constexpr UInt16 type() const { return type_; }

private:
	const Big16 type_;
};

static_assert(IsLiteralType<InstructionType>(), "Literal type expected.");

} // </namespace ofp>

#endif // OFP_INSTRUCTIONTYPE_H
