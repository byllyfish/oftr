#ifndef OFP_PROTOCOLITERATOR_H
#define OFP_PROTOCOLITERATOR_H

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp { // <namespace ofp>

template <class ElemType>
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

/**
  /// \returns Number of elements between begin and end.
  static size_t distance(const ProtocolIterator &begin,
                         const ProtocolIterator &end) {
    assert(begin <= end);

    size_t dist = 0;
    while (begin < end) {
      ++dist;
      ++begin;
    }
    assert(begin == end);

    return dist;
  }
**/

private:
  const UInt8 *pos_;

  ProtocolIterator(const UInt8 *pos) : pos_{pos} {}

  template <class T>
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
  // TODO
};

} // </namespace ofp>

#endif // OFP_PROTOCOLITERATOR_H
