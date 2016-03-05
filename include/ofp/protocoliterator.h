// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

///   ProtocolIterator<class ElemType>
///   ProtocolIteratorFixedType<class ElemType>
///   ProtocolIteratorFixedTypeAndSize<class ElemType>
///
/// Common Synopsis:
///
/// class ProtocolIteratorX<class ElemType> {
/// public:
///   using Item = ElemType;
///
///   const ElemType &operator*() const;
///   const ElemType *operator->() const;
///
///   const UInt8 *data() const;
///   size_t size() const;
///
///   const UInt8 *valuePtr() const;
///   size_t valueSize() const;
///
///   void operator++();
///
///   bool operator==(const ProtocolIterator &rhs) const;
///   bool operator!=(const ProtocolIterator &rhs) const;
///   bool operator<(const ProtocolIterator &rhs) const;
///   bool operator<=(const ProtocolIterator &rhs) const;
///   bool operator>(const ProtocolIterator &rhs) const;
///   bool operator>=(const ProtocolIterator &rhs) const;
/// };

#ifndef OFP_PROTOCOLITERATOR_H_
#define OFP_PROTOCOLITERATOR_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/byterange.h"
#include "ofp/constants.h"

namespace ofp {

class Validation;

namespace detail {

/// Return true if given ByteRange is a valid protocol range.
///
/// To be structurally valid as a ProtocolRange, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of 8.
///   - no element length is less than `minElemSize`.
///   - the sum of each element length (padded out to 8-bytes) evenly matches
///     the size.
///   - pointer to start of range is 8-byte aligned.
///
/// \return true if byte range is a valid protocol iterable.
bool IsProtocolRangeValid(
    size_t elementSize, const ByteRange &range, size_t sizeFieldOffset,
    size_t alignment, Validation *context,
    ProtocolIteratorType iterType = ProtocolIteratorType::Unspecified);

/// Return count of items in the protocol range.
///
/// If protocol iterable is not valid, return count of valid items.
///
/// \return number of items in iterable.
size_t ProtocolRangeItemCount(size_t elementSize, const ByteRange &range,
                              size_t sizeFieldOffset, size_t alignment = 8);

/// Return byte offset where the protocol range can be safely split before
/// it exceeds the given `chunkSize`.
///
/// \return byte offset <= chunkSize
size_t ProtocolRangeSplitOffset(size_t chunkSize, size_t elementSize,
                                const ByteRange &range, size_t sizeFieldOffset);

template <class ElemType, size_t SizeOffset>
struct ProtocolElement {
  static size_t size(const UInt8 *ptr) {
    static_assert(SizeOffset < 32, "Unexpected value of size offset.");
    return std::max<size_t>(sizeof(Big16) + SizeOffset,
                            *Big16_cast(ptr + SizeOffset));
  }
};

template <class ElemType>
struct ProtocolElement<ElemType, PROTOCOL_ITERATOR_SIZE_FIXED> {
  static size_t size(const UInt8 *ptr) { return sizeof(ElemType); }
};

template <class ElemType>
struct ProtocolElement<ElemType, PROTOCOL_ITERATOR_SIZE_CONDITIONAL> {
  static size_t size(const UInt8 *ptr) {
    return *Big16_cast(ptr) == 0xffff ? 8 : 4;
  }
};

}  // namespace detail

// ProtocolIterator is a template for an iterator that traverses an array of
// variable-sized elements. The structure of each element header is:
//
//    Big16 type;
//    Big16 length;  (includes length of header (4))
//    ...
//
// The actual element is padded to a multiple of 8 bytes.

template <class ElemType,
          ProtocolIteratorType IterType = ProtocolIteratorType::Unspecified,
          size_t SizeOffset = ElemType::ProtocolIteratorSizeOffset,
          size_t Alignment = ElemType::ProtocolIteratorAlignment>
class ProtocolIterator {
 public:
  static_assert((SizeOffset == PROTOCOL_ITERATOR_SIZE_FIXED) ||
                    (SizeOffset == PROTOCOL_ITERATOR_SIZE_CONDITIONAL) ||
                    (SizeOffset < 32),
                "Unexpected SizeOffset.");
  static_assert((Alignment == 8) || (Alignment == 4), "Unexpected alignment.");

  // Define types for std::iterator.
  using difference_type = ptrdiff_t;
  using value_type = ElemType;
  using reference = const ElemType &;
  using pointer = const ElemType *;
  using iterator_category = std::forward_iterator_tag;

  using Element = ElemType;

  const ElemType &operator*() const { return *operator->(); }

  const ElemType *operator->() const {
    return Interpret_cast<ElemType>(data());
  }

  const UInt8 *data() const {
    assert(IsPtrAligned(pos_, Alignment));
    return pos_;
  }

  /// \returns size of element including type and length fields.
  size_t size() const {
    return detail::ProtocolElement<ElemType, SizeOffset>::size(data());
  }

  void operator++() { pos_ += (Alignment == 8) ? PadLength(size()) : size(); }

  bool operator==(const ProtocolIterator &rhs) const {
    return pos_ == rhs.pos_;
  }

  bool operator!=(const ProtocolIterator &rhs) const {
    return pos_ != rhs.pos_;
  }

  bool operator<=(const ProtocolIterator &rhs) const {
    return pos_ <= rhs.pos_;
  }

  bool operator<(const ProtocolIterator &rhs) const { return pos_ < rhs.pos_; }

  bool operator>(const ProtocolIterator &rhs) const { return pos_ > rhs.pos_; }

  bool operator>=(const ProtocolIterator &rhs) const {
    return pos_ >= rhs.pos_;
  }

 private:
  const UInt8 *pos_;

  // Only a ProtocolRange or ProtocolList can construct a ProtocolIterator.
  explicit ProtocolIterator(const UInt8 *pos) : pos_{pos} {
    assert(IsPtrAligned(pos, Alignment));
  }

  static size_t itemCount(const ByteRange &range) {
    return detail::ProtocolRangeItemCount(sizeof(ElemType), range, SizeOffset,
                                          Alignment);
  }

  static bool isValid(const ByteRange &range, Validation *context) {
    return detail::IsProtocolRangeValid(sizeof(ElemType), range, SizeOffset,
                                        Alignment, context, IterType);
  }

  template <class I>
  friend class ProtocolRange;

  template <class R>
  friend class ProtocolList;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLITERATOR_H_
