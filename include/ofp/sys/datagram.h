// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_DATAGRAM_H_
#define OFP_SYS_DATAGRAM_H_

#include "ofp/bytelist.h"

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

class Datagram {
 public:
  const UInt8 *data() const { return buf_.data(); }
  size_t size() const { return buf_.size(); }

  udp::endpoint destination() const { return dest_; }
  void setDestination(const udp::endpoint &dest) { dest_ = dest; }

  UInt64 connectionId() const { return connId_; }
  void setConnectionId(UInt64 connId) { connId_ = connId; }

  void write(const void *data, size_t length) { buf_.add(data, length); }

 private:
  ByteList buf_;
  udp::endpoint dest_;
  UInt64 connId_ = 0;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_DATAGRAM_H_
