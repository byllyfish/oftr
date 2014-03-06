#ifndef OFP_PROTOCOLITERATOR_H_
#define OFP_PROTOCOLITERATOR_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/byterange.h"

namespace ofp {
namespace detail {

/// Return true if given ByteRange is a valid protocol iterable.
///
/// To be structurally valid as a ProtocolIterable, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of 8.
///   - no element length is less than 4.
///   - the sum of each element length (padded out to 8-bytes) evenly matches
///     the size.
///   - pointer to start of range is 8-byte aligned.
///
/// \return true if byte range is a valid protocol iterable.
bool IsProtocolIteratorValid(const ByteRange &range, const char *context="");

/// Return count of items in the protocol iterable.
///
/// If protocol iterable is not valid, return count of valid items.
///
/// \return number of items in iterable.
size_t ProtocolIteratorItemCount(const ByteRange &range);

/// Return true if given ByteRange is a valid protocol iterable for an element
/// with a fixed size.
/// 
/// To be structurally valid as a ProtocolIterable, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of elemSize
///   - pointer to start of range is 8-byte aligned.
/// 
/// \return true if byte range is a valid protocol iterable.
bool IsProtocolIteratorFixedSizeAndTypeValid(size_t elemSize, const ByteRange &range, const char *context="");

/// Return count of items in the protocol iterable.
/// 
/// \return number of items in iterable.
size_t ProtocolIteratorFixedSizeAndTypeItemCount(size_t elemSize, const ByteRange &range);

}  // namespace detail

template <class ElemType, class Iterator>
class ProtocolIterable;

// ProtocolIterator is a template for an iterator that traverses an array of
// variable-sized elements. The structure of each element header is:
//
//    Big16 type;
//    Big16 length;  (includes length of header (4))
//    ...
//
// The actual element is padded to a multiple of 8 bytes.

template <class ElemType>
class ProtocolIterator {
public:

  const ElemType &operator*() const {
    return *operator->();
  }

  const ElemType *operator->() const {
    return reinterpret_cast<const ElemType *>(data());
  }

  const UInt8 *data() const {
    assert(IsPtrAligned<8>(pos_));
    return pos_;
  }

  /// \returns size of element including type and length fields. Minimum
  /// length is 4.
  size_t size() const { return std::min<size_t>(4, *Big16_cast(data() + 2)); }

  const UInt8 *valuePtr() const { return data() + 4; }
  size_t valueSize() const { return size() - 4; }

  void operator++() { pos_ += PadLength(size()); }

  bool operator==(const ProtocolIterator &rhs) const {
    return data() == rhs.data();
  }

  bool operator!=(const ProtocolIterator &rhs) const {
    return data() != rhs.data();
  }

  bool operator<=(const ProtocolIterator &rhs) const {
    return data() <= rhs.data();
  }

  bool operator<(const ProtocolIterator &rhs) const {
    return data() < rhs.data();
  }

  bool operator>(const ProtocolIterator &rhs) const {
    return data() > rhs.data();
  }

  bool operator>=(const ProtocolIterator &rhs) const {
    return data() >= rhs.data();
  }

private:
  const UInt8 *pos_;

  // Only a ProtocolIterable<*> can construct a ProtocolIterator.
  ProtocolIterator(const UInt8 *pos) : pos_{pos} {}

  static size_t rangeItemCount(const ByteRange &range) {
    return detail::ProtocolIteratorItemCount(range);
  }

  static bool isRangeValid(const ByteRange &range, const char *context) {
    return detail::IsProtocolIteratorValid(range, context);
  }

  template <class T, class I>
  friend class ProtocolIterable;
};


// ProtocolIteratorFixedType is a template for an iterator that traverses an
// array of variable-sized elements. The structure of the element header is:
//
//    Big16 length;   (includes length of header (2))
//    ...
//
// The actual element is padded to a multiple of 8 bytes.

template <class ElemType>
class ProtocolIteratorFixedType {
public:

  const ElemType &operator*() const {
    return *operator->();
  }

  const ElemType *operator->() const {
    return reinterpret_cast<const ElemType *>(data());
  }

  const UInt8 *data() const {
    assert(IsPtrAligned<8>(pos_));
    return pos_;
  }

  // TODO(bfish) - finish the rest...

private:
  const UInt8 *pos_;
};

// ProtocolIteratorFixedSizeAndType is a template for an iterator that traverses
// an array of fixed-size elements. There is no element header. The actual 
// element size must be a multiple of 8 bytes.

template <class ElemType>
class ProtocolIteratorFixedSizeAndType {
public:
  static_assert((sizeof(ElemType) % 8) == 0, "Unexpected element size.");

  const ElemType &operator*() const {
    return *operator->();
  }

  const ElemType *operator->() const {
    return reinterpret_cast<const ElemType *>(data());
  }

  const UInt8 *data() const {
    assert(IsPtrAligned<8>(pos_));
    return pos_;
  }

  /// \returns size of element.
  size_t size() const { return sizeof(ElemType); }

  void operator++() { pos_ += size(); }

  bool operator==(const ProtocolIteratorFixedSizeAndType &rhs) const {
    return data() == rhs.data();
  }

  bool operator!=(const ProtocolIteratorFixedSizeAndType &rhs) const {
    return data() != rhs.data();
  }

  bool operator<=(const ProtocolIteratorFixedSizeAndType &rhs) const {
    return data() <= rhs.data();
  }

  bool operator<(const ProtocolIteratorFixedSizeAndType &rhs) const {
    return data() < rhs.data();
  }

  bool operator>(const ProtocolIteratorFixedSizeAndType &rhs) const {
    return data() > rhs.data();
  }

  bool operator>=(const ProtocolIteratorFixedSizeAndType &rhs) const {
    return data() >= rhs.data();
  }

private:
  const UInt8 *pos_;

  // Only a ProtocolIterable<*> can construct a ProtocolIterator.
  ProtocolIteratorFixedSizeAndType(const UInt8 *pos) : pos_{pos} {}

  static size_t rangeItemCount(const ByteRange &range) {
    return detail::ProtocolIteratorFixedSizeAndTypeItemCount(sizeof(ElemType), range);
  }

  static bool isRangeValid(const ByteRange &range, const char *context) {
    return detail::IsProtocolIteratorFixedSizeAndTypeValid(sizeof(ElemType), range, context);
  }

  template <class T, class I>
  friend class ProtocolIterable;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLITERATOR_H_
