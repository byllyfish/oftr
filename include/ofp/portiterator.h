#ifndef OFP_PORTITERATOR_H_
#define OFP_PORTITERATOR_H_

#include "ofp/port.h"

namespace ofp {

class PortIterator {
 public:
  using Item = Port;

  const Item &operator*() const {
    return *reinterpret_cast<const Item *>(position_);
  }

  const UInt8 *data() const { return position_; }
  UInt16 size() const { return sizeof(Port); }

  // No operator -> (FIXME?)
  // No postfix ++

  void operator++() {
    auto sz = size();
    assert(sz >= 4);
    position_ += sz;
  }

  bool operator==(const PortIterator &rhs) const {
    return position_ == rhs.position_;
  }

  bool operator!=(const PortIterator &rhs) const { return !(*this == rhs); }

  bool operator<=(const PortIterator &rhs) const {
    return position_ <= rhs.position_;
  }

  bool operator<(const PortIterator &rhs) const {
    return position_ < rhs.position_;
  }

  /// \returns Number of instructions between begin and end. 
  static size_t distance(PortIterator begin, PortIterator end) {
    assert(begin <= end);
    // TODO(bfish): use pointer subtraction.
    size_t dist = 0;
    while (begin < end) {
      ++dist;
      ++begin;
    }
    return dist;
  }

 private:
  const UInt8 *position_;

  explicit PortIterator(const UInt8 *pos) : position_{pos} {}

  friend class PortRange;
};

}  // namespace ofp

#endif  // OFP_PORTITERATOR_H_
