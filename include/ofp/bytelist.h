#ifndef OFP_BYTELIST_H
#define OFP_BYTELIST_H

#include "ofp/byterange.h"
#include <vector>

namespace ofp { // <namespace ofp>

/**
 *  ByteList is concrete class representing a mutable byte buffer. This class
 *  manages its own memory. The memory underlying a ByteList is relocatable;
 *  methods that change the size of the buffer can cause pointers into the
 *  ByteList to become invalid.
 *
 *  When you copy or assign a ByteList, it produces a deep copy.
 *
 *  This is a cornerstone class so there are a lot of assert() checks.
 *  
 *  @see ByteData
 *  @see ByteRange
 */
class ByteList {
public:
    ByteList() = default;
    ByteList(const void *data, size_t length);

    const UInt8 *begin() const;
    const UInt8 *end() const;

    const UInt8 *data() const;
    UInt8 *mutableData();

    size_t size() const;
    ptrdiff_t index(const UInt8 *pos) const;
    ByteRange toRange() const;

    const UInt8 &operator[](size_t idx) const;
    UInt8 &operator[](size_t idx);

    void add(const void *data, size_t length);
    void insert(const UInt8 *pos, const void *data, size_t length);
    void replace(const UInt8 *pos, const UInt8 *posEnd, const void *data,
                 size_t length);

    void addUninitialized(size_t length);
    void insertUninitialized(const UInt8 *pos, size_t length);
    void replaceUninitialized(const UInt8 *pos, const UInt8 *posEnd,
                              size_t length);

    void resize(size_t length);
    void clear();

private:
    std::vector<UInt8> buf_;

    void assertInRange(const UInt8 *pos) const;
};

std::ostream &operator<<(std::ostream &os, const ByteList &value);

} // </namespace ofp>


/**
 *  Construct mutable byte list containing the specified data.
 */
inline ofp::ByteList::ByteList(const void *data, size_t length)
{
	add(data, length);
}


/**
 *  Return pointer to beginning of byte buffer.
 */
inline const ofp::UInt8 *ofp::ByteList::begin() const
{
    return &buf_[0];
}

/**
 *  Return pointer to end of byte buffer (i.e. one past last byte).
 */
inline const ofp::UInt8 *ofp::ByteList::end() const
{
    return &buf_[buf_.size()];
}

/**
 *  Return pointer to beginning of byte buffer.
 */
inline const ofp::UInt8 *ofp::ByteList::data() const
{
    return &buf_[0];
}

/**
 *  Return pointer to beginning of byte buffer. Contents of buffer may be
 *  changed.
 */
inline ofp::UInt8 *ofp::ByteList::mutableData()
{
	return &buf_[0];
}


/**
 *  Return size of byte buffer.
 */
inline size_t ofp::ByteList::size() const
{
    return buf_.size();
}

/**
 *  Return index of specified pointer in byte byffer.
 */
inline ptrdiff_t ofp::ByteList::index(const UInt8 *pos) const
{
    assertInRange(pos);

    return pos - &buf_[0];
}

/**
 *  Return range of bytes.
 */
inline ofp::ByteRange ofp::ByteList::toRange() const
{
	return ByteRange{begin(), end()};
}

/**
 *  Return immutable byte at specified index.
 */
inline const ofp::UInt8 &ofp::ByteList::operator[](size_t idx) const
{
    assert(idx < size());

    return buf_[idx];
}

/**
 *  Return mutable byte at specified index.
 */
inline ofp::UInt8 &ofp::ByteList::operator[](size_t idx)
{
    assert(idx < size());

    return buf_[idx];
}

/**
 *  Append data to the end of the byte buffer. This method may move memory.
 */
inline void ofp::ByteList::add(const void *data, size_t length)
{
    assert(data != nullptr || length == 0);

    auto bp = BytePtr(data);
    buf_.insert(buf_.end(), bp, bp + length);
}

/**
 *  Insert bytes into the beginning, middle, or end of the byte buffer. This
 *  method may move memory.
 */
inline void ofp::ByteList::insert(const UInt8 *pos, const void *data,
                                  size_t length)
{
    assertInRange(pos);
    assert(data != nullptr || length == 0);

    auto bp = BytePtr(data);
    buf_.insert(buf_.begin() + index(pos), bp, bp + length);
}

/**
 *  Replace bytes in the byte buffer with new values. This method may move
 *  memory.
 */
inline void ofp::ByteList::replace(const UInt8 *pos, const UInt8 *posEnd,
                                   const void *data, size_t length)
{
    assert(data != nullptr || length == 0);

    replaceUninitialized(pos, posEnd, length);

    // N.B. Memory might have moved.
    auto bp = BytePtr(data);
    std::copy(bp, bp + length, buf_.begin() + index(pos));
}

/**
 *  Append uninitialized bytes to the end of the byte buffer. This method may
 *  move memory.
 */
inline void ofp::ByteList::addUninitialized(size_t length)
{
    buf_.insert(buf_.end(), length, 0);
}

/**
 *  Insert uninitialized bytes into the beginning, middle, or end of the byte
 *  buffer. This method may move memory.
 */
inline void ofp::ByteList::insertUninitialized(const UInt8 *pos, size_t length)
{
    assertInRange(pos);

    buf_.insert(buf_.begin() + index(pos), length, 0);
}

/**
 *  Replace uninitialized bytes in the byte buffer with new values. This method
 *  may move memory.
 */
inline void ofp::ByteList::replaceUninitialized(const UInt8 *pos,
                                                const UInt8 *posEnd,
                                                size_t length)
{
    assert(pos <= posEnd);
    assertInRange(pos);
    assertInRange(posEnd);

    size_t oldlen = Unsigned_cast(posEnd - pos);

    if (length > oldlen) {
        buf_.insert(buf_.begin() + index(posEnd), length - oldlen, 0);
    } else if (length < oldlen) {
        auto iter = buf_.begin() + index(pos);
        buf_.erase(iter, iter + Signed_cast(oldlen - length));
    }
}

/**
 *  Resize buffer to specified length.
 */
inline void ofp::ByteList::resize(size_t length)
{
    buf_.resize(length);
}


/**
 *  Set size of byte buffer to zero.
 */
inline void ofp::ByteList::clear()
{
    buf_.clear();
}

/**
 *  Helper function to verify pointer points into byte buffer.
 */
inline void ofp::ByteList::assertInRange(const UInt8 *pos) const
{
    assert(pos != nullptr && "position is null");
    assert((pos >= begin() && pos <= end()) && "position out of range");
}

/**
 *  Write buffer to stream in hexadecimal format.
 */
inline std::ostream &ofp::operator<<(std::ostream &os, const ByteList &value)
{
    return os << RawDataToHex(value.data(), value.size(), ' ', 2);
}

#endif // OFP_BYTELIST_H
