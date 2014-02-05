#ifndef OFP_PORTRANGE_H_
#define OFP_PORTRANGE_H_

#include "ofp/byterange.h"
#include "ofp/portiterator.h"

namespace ofp {

class PortList;
class Writable;

class PortRange {
 public:
  PortRange() = default;
  PortRange(const ByteRange &range) : range_{range} {}
  PortRange(const PortList &ports);

  size_t itemCount() const { return PortIterator::distance(begin(), end()); }

  PortIterator begin() const {
    return PortIterator{range_.begin()};
  }

  PortIterator end() const {
    return PortIterator{range_.end()};
  }

  const UInt8 *data() const { return range_.data(); }
  size_t size() const { return range_.size(); }
  ByteRange toByteRange() const { return range_; }

  /// \returns Size of port list when written to channel using the specified
  /// protocol version.
  size_t writeSize(Writable *channel);

  /// \brief Writes port list to the channel using the specified protocol
  /// version.
  void write(Writable *channel);

 private:
  ByteRange range_;
};

}  // namespace ofp

#endif  // OFP_PORTRANGE_H_
