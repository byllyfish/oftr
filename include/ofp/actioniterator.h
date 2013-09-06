#ifndef OFP_ACTIONITERATOR_H
#define OFP_ACTIONITERATOR_H

#include "ofp/actiontype.h"

namespace ofp { // <namespace ofp>

class ActionIterator {
public:
	
	ActionType type() const {
		return ActionType::fromBytes(position_);
	}
	
	const UInt8 *data() const { return position_; }

	template <class Type>
	const Type *action() {
		return reinterpret_cast<const Type *>(position_);
	}
	
	// No operator -> (FIXME?)
	// No postfix ++
	
	void operator++() 
	{
		position_ += type().length();
	}
		
	bool operator==(const ActionIterator &rhs) const {
		return position_ == rhs.position_;
	}
	
	bool operator!=(const ActionIterator &rhs) const {
		return !(*this == rhs);
	}

private:
	const UInt8 *position_;

	explicit ActionIterator(const void *pos)
		: position_{static_cast<const UInt8 *>(pos)} {}
		
	friend class ActionRange;
};

} // </namespace ofp>

#endif // OFP_ACTIONITERATOR_H
