// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PORTLIST_H_
#define OFP_PORTLIST_H_

#include "ofp/protocollist.h"
#include "ofp/portrange.h"

namespace ofp {

class PortList : public ProtocolList<PortRange> {
 public:
  void add(const PortBuilder &port) {
    const PropertyList &props = port.properties_;
    assert(port.msg_.length_ ==
           PortBuilder::SizeWithoutProperties + props.size());
    buf_.add(&port.msg_, PortBuilder::SizeWithoutProperties);
    buf_.add(props.data(), props.size());
  }
};

}  // namespace ofp

#endif  // OFP_PORTLIST_H_
