#ifndef OFP_INSTRUCTIONITERATOR_H
#define OFP_INSTRUCTIONITERATOR_H

#include "ofp/instructiontype.h"

namespace ofp { // <namespace ofp>

class InstructionIterator {
public:

	InstructionType type() const {
		return InstructionType::fromBytes(position_);
	}
	
	const UInt8 *data() const { return position_; }
	UInt16 size() const { return *reinterpret_cast<const Big16*>(data() + sizeof(InstructionType)); }

	template <class Type>
	const Type *instruction() {
		return reinterpret_cast<const Type *>(position_);
	}

	// No operator -> (FIXME?)
	// No postfix ++
	
	void operator++() 
	{
		position_ += size();
	}
		
	bool operator==(const InstructionIterator &rhs) const {
		return position_ == rhs.position_;
	}
	
	bool operator!=(const InstructionIterator &rhs) const {
		return !(*this == rhs);
	}

private:
	const UInt8 *position_;

	explicit InstructionIterator(const void *pos)
		: position_{static_cast<const UInt8 *>(pos)} {}

	friend class InstructionList;
};

} // </namespace ofp>

#endif // OFP_INSTRUCTIONITERATOR_H
