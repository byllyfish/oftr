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

        ActionType type() const
        {
            return position().type();
        }

        template <class Type>
        const Type *action()
        {
            return position().action<Type>();
        }

        OXMIterator oxmIterator() const
        {
            return position().oxmIterator();
        }

        ActionIterator position() const
        {
            return ActionIterator{BytePtr(this)};
        }

    private:
        Item() = default;
    };

	explicit ActionIterator(const UInt8 *pos)
        : position_{pos}
    {
    }

    const Item &operator*() const
    {
        return *reinterpret_cast<const Item *>(position_);
    }

    ActionType type() const
    {
        return ActionType::fromBytes(position_);
    }

    const UInt8 *data() const
    {
        return position_;
    }

    template <class Type>
    const Type *action()
    {
        return reinterpret_cast<const Type *>(position_);
    }

    OXMIterator oxmIterator() const {
        return OXMIterator{BytePtr(position_) + sizeof(ActionType)};
    }

    const UInt8 *valuePtr() const
    {
        return data() + sizeof(ActionType);
    }

    size_t valueSize() const
    {
        return type().length() - sizeof(ActionType);
    }
    // No operator -> (FIXME?)
    // No postfix ++

    void operator++()
    {
        position_ += *reinterpret_cast<const Big_unaligned<UInt16> *>(
                          position_ + 2); // type().length();
    }

    bool operator==(const ActionIterator &rhs) const
    {
        return position_ == rhs.position_;
    }

    bool operator!=(const ActionIterator &rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<=(const ActionIterator &rhs) const
    {
        return position_ <= rhs.position_;
    }

    bool operator<(const ActionIterator &rhs) const
    {
    	return position_ < rhs.position_;
    }

    /// \returns Number of actions between begin and end.
    static size_t distance(ActionIterator begin, ActionIterator end)
    {
        assert(begin <= end);

        size_t dist = 0;
        while (begin < end) {
            ++dist;
            ++begin;
        }
        assert(begin == end);
        
        return dist;
    }


private:
    const UInt8 *position_;
};

} // </namespace ofp>

#endif // OFP_ACTIONITERATOR_H