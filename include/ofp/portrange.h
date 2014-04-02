#ifndef OFP_PORTRANGE_H_
#define OFP_PORTRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/port.h"

namespace ofp {

class PortList;
class Writable;

using PortIterator = ProtocolIterator<Port>;

class PortRange : public ProtocolRange<PortIterator> {
public:
  PortRange() = default;
  PortRange(const ByteRange &range) : ProtocolRange{range} {}
  PortRange(const PortList &ports);

  /// \returns Size of port list when written to channel using the specified
  /// protocol version.
  size_t writeSize(Writable *channel);

  /// \brief Writes port list to the channel using the specified protocol
  /// version.
  void write(Writable *channel);  
};

}  // namespace ofp

#endif  // OFP_PORTRANGE_H_
