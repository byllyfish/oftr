#ifndef OFP_BYTERANGE_H
#define OFP_BYTERANGE_H

#include "ofp/types.h"

namespace ofp { // <namespace ofp>

class ByteRange {
public:
	constexpr ByteRange();
	constexpr ByteRange(const void *data, size_t length);

	constexpr const UInt8 *data() const;
	constexpr size_t size() const;

	bool operator==(const ByteRange &rhs) const;
	bool operator!=(const ByteRange &rhs) const;

private:
	const UInt8 *begin_;
	const UInt8 *end_;
};

} // </namespace ofp>


//constexpr ofp::ByteRange::ByteRange(const UInt8 *begin, const UInt8 *end) : begin_{begin}, end_{end} {}

constexpr ofp::ByteRange::ByteRange() : begin_{nullptr}, end_{nullptr} {}

constexpr ofp::ByteRange::ByteRange(const void *data, size_t length) : begin_{BytePtr(data)}, end_{BytePtr(data) + length} {}

constexpr const ofp::UInt8 *ofp::ByteRange::data() const
{
	return begin_;
}

constexpr size_t ofp::ByteRange::size() const
{
	return Unsigned_cast(end_ - begin_);
}


inline bool ofp::ByteRange::operator==(const ByteRange &rhs) const
{
	return size() == rhs.size() && std::equal(begin_, end_, rhs.data());
}

inline bool ofp::ByteRange::operator!=(const ByteRange &rhs) const
{
	return !(*this == rhs);
}

#endif // OFP_BYTERANGE_H
