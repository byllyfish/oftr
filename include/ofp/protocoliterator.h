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

namespace ofp {
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
bool IsProtocolRangeValid(const ByteRange &range, size_t sizeFieldOffset, size_t alignment = 8, const char *context="", size_t minElemSize = 4);

/// Return count of items in the protocol range.
///
/// If protocol iterable is not valid, return count of valid items.
///
/// \return number of items in iterable.
size_t ProtocolRangeItemCount(const ByteRange &range, size_t sizeFieldOffset, size_t alignment = 8);

enum { ProtocolIteratorSizeOffsetCutoff = 128 };

/// Return true if given ByteRange is a valid protocol iterable for an element
/// with a fixed size.
/// 
/// To be structurally valid as a ProtocolRange, the byte range must pass
/// the following tests:
///   - size in bytes is a multiple of elemSize
///   - pointer to start of range is 8-byte aligned.
/// 
/// \return true if byte range is a valid protocol iterable.
bool IsProtocolRangeFixedValid(size_t elemSize, const ByteRange &range, const char *context="");

/// Return count of items in the protocol iterable.
/// 
/// \return number of items in iterable.
size_t ProtocolRangeFixedItemCount(size_t elemSize, const ByteRange &range);


}  // namespace detail

// ProtocolIterator is a template for an iterator that traverses an array of
// variable-sized elements. The structure of each element header is:
//
//    Big16 type;
//    Big16 length;  (includes length of header (4))
//    ...
//
// The actual element is padded to a multiple of 8 bytes.

template <class ElemType, size_t SizeOffset=ElemType::ProtocolIteratorSizeOffset, size_t Alignment=8>
class ProtocolIterator {
public:
  enum { IsFixedSize = (SizeOffset > detail::ProtocolIteratorSizeOffsetCutoff) };
  enum { MinSize  = IsFixedSize ? 0 : sizeof(Big16) + SizeOffset };
  
  static_assert(sizeof(ElemType) >= MinSize, "Unexpected element size.");

  // Define types for std::iterator.
  using difference_type = ptrdiff_t;
  using value_type = ElemType;
  using reference = const ElemType&;
  using pointer = const ElemType*;
  using iterator_category = std::forward_iterator_tag;

  using Element = ElemType;

  const ElemType &operator*() const {
    return *operator->();
  }

  const ElemType *operator->() const {
    return reinterpret_cast<const ElemType *>(data());
  }

  const UInt8 *data() const {
    assert(IsPtrAligned(pos_, Alignment));
    return pos_;
  }

  /// \returns size of element including type and length fields.
  size_t size() const { 
    return IsFixedSize ?  sizeof(ElemType) : std::max<size_t>(MinSize, *Big16_cast(data() + SizeOffset)); 
  }

  void operator++() { pos_ += (Alignment == 8) ? PadLength(size()) : size(); }

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

  // Only a ProtocolRange or ProtocolList can construct a ProtocolIterator.
  ProtocolIterator(const UInt8 *pos) : pos_{pos} {}

  static size_t itemCount(const ByteRange &range) {
    return IsFixedSize ? detail::ProtocolRangeFixedItemCount(sizeof(ElemType), range) :
     detail::ProtocolRangeItemCount(range, SizeOffset, Alignment);
  }

  static bool isValid(const ByteRange &range, const char *context) {
    return IsFixedSize ? detail::IsProtocolRangeFixedValid(sizeof(ElemType), range, context) :
     detail::IsProtocolRangeValid(range, SizeOffset, Alignment, context);
  }

  template <class I>
  friend class ProtocolRange;

  template <class R>
  friend class ProtocolList;
};

}  // namespace ofp

#endif  // OFP_PROTOCOLITERATOR_H_
