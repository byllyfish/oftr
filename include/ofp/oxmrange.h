//  ===== ---- ofp/oxmrange.h ------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the OXMRange class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_OXMRANGE_H
#define OFP_OXMRANGE_H

#include "ofp/oxmiterator.h"
#include <ostream>

namespace ofp { // <namespace ofp>

class OXMRange {
public:

	constexpr OXMRange() : begin_{nullptr}, end_{nullptr} {}
	
    constexpr OXMRange(OXMIterator begin, OXMIterator end)
        : begin_{begin.data()}, end_{end.data()}
    {
    }

    constexpr OXMRange(const void *data, size_t length)
        : begin_{static_cast<const UInt8 *>(data)},
          end_{static_cast<const UInt8 *>(data) + length}
    {
    }

    // Backwards consructor used in compiling prereqs.
    constexpr OXMRange(size_t length, const UInt8 data[])
        : begin_{data}, end_{data + length}
    {
    }

    template <class Value>
    typename Value::NativeType get() const;

    OXMIterator begin() const
    {
        return OXMIterator{begin_};
    }
    OXMIterator end() const
    {
        return OXMIterator{end_};
    }

    const UInt8 *data() const
    {
        return begin_;
    }
    size_t size() const
    {
        return Unsigned_cast(end_ - begin_);
    }

    bool operator==(const OXMRange &rhs) const
    {
        return size() == rhs.size() &&
               0 == std::memcmp(data(), rhs.data(), size());
    }

    bool operator!=(const OXMRange &rhs) const
    {
        return !(*this == rhs);
    }

    bool validateLength() const
    {
        assert(begin_ <= end_);
        const UInt8 *pos = begin_;
        int left = static_cast<int>(end_ - begin_);
        while (pos < end_) {
            if (left < 4)
                return false;
            UInt8 len = sizeof(OXMType) + pos[3];
            if (left < len)
                return false;
            pos += len;
            left -= len;
        }
        assert(pos == end_);
        assert(left == 0);
        return true;
    }

private:
    const UInt8 *begin_;
    const UInt8 *end_;
};

template <class Value>
typename Value::NativeType OXMRange::get() const
{
	using NativeType = typename Value::NativeType;

	for (auto &item : *this) {
		if (item.type() == Value::type()) {
			return item.value<Value>();
		}
	}

	return NativeType{};
}

std::ostream &operator<<(std::ostream &stream, const OXMRange &range);

inline std::ostream &operator<<(std::ostream &stream, const OXMRange &range)
{
    return stream << "[OXMRange size=" << range.size()
                  << " data=" << RawDataToHex(range.data(), range.size())
                  << ']';
}

} // </namespace ofp>

#endif // OFP_OXMRANGE_H
