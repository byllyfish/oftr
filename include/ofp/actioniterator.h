#ifndef OFP_ACTIONITERATOR_H
#define OFP_ACTIONITERATOR_H

#include "ofp/actiontype.h"
#include "ofp/oxmiterator.h"

namespace ofp { // <namespace ofp>

class ActionIterator {
public:
	
	class Item {
	public:
		
		Item(const Item &) = delete;
		Item &operator=(const Item &) = delete;

		ActionType type() const {
			return position().type();
		}

		template <class Type>
		const Type *action() {
			return position().action<Type>();
		}

		OXMIterator oxmIterator() const {
			return OXMIterator{BytePtr(this) + sizeof(ActionType)};
		}

		ActionIterator position() const { return ActionIterator{BytePtr(this)}; }
		
	private:
		Item() = default;
	};

	const Item &operator*() const 
	{
		return *reinterpret_cast<const Item *>(position_);
	}

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

	bool operator<=(const ActionIterator &rhs) const {
		return position_ <= rhs.position_;
	}

	/// \returns Number of actions between begin and end.
	static size_t distance(ActionIterator begin, ActionIterator end)
	{
		assert(begin <= end);
		
		size_t dist = 0;
		while (begin != end) {
			++dist;
			++begin;
		}
		return dist;
	}

private:
	const UInt8 *position_;

	explicit ActionIterator(const void *pos)
		: position_{static_cast<const UInt8 *>(pos)} {}
		
	friend class ActionRange;
};

} // </namespace ofp>

#endif // OFP_ACTIONITERATOR_H
