// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PORTLIST_H_
#define OFP_PORTLIST_H_

#include "ofp/protocollist.h"
#include "ofp/portrange.h"

namespace ofp {

class PortList : public ProtocolList<PortRange> {
 public:
  void add(const Port &port) { buf_.add(&port, sizeof(Port)); }
  void add(const PortBuilder &port) { add(port.toPort()); }

  // PortRange toRange() const { return buf_.toRange(); }
  // void operator=(const PortRange &range) { buf_ = range.toByteRange(); }

  // private:
  //  ByteList buf_;
};

}  // namespace ofp

#endif  // OFP_PORTLIST_H_
