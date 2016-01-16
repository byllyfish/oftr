// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PORTRANGE_H_
#define OFP_PORTRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/port.h"

namespace ofp {

class PortList;
class Writable;

using PortIterator = ProtocolIterator<Port, ProtocolIteratorType::Port>;

class PortRange : public ProtocolRange<PortIterator> {
  using Inherited = ProtocolRange<PortIterator>;

 public:
  using Inherited::Inherited;

  /* implicit NOLINT */ PortRange(const PortList &ports);

  /// \returns Size of port list when written to channel using the specified
  /// protocol version.
  size_t writeSize(Writable *channel);

  /// \brief Writes port list to the channel using the specified protocol
  /// version.
  void write(Writable *channel);
};

}  // namespace ofp

#endif  // OFP_PORTRANGE_H_
