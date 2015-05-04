#ifndef OFP_MESSAGEINFO_H_
#define OFP_MESSAGEINFO_H_

#include "ofp/ipv6endpoint.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class MessageInfo {
 public:
  MessageInfo() = default;
  explicit MessageInfo(UInt64 sessionId, const IPv6Endpoint &src,
                       const IPv6Endpoint &dst)
      : sessionId_{sessionId}, source_{src}, dest_{dst} {}

  UInt64 sessionId() const { return sessionId_; }
  IPv6Endpoint source() const { return source_; }
  IPv6Endpoint dest() const { return dest_; }

 private:
  UInt64 sessionId_ = 0;
  IPv6Endpoint source_;
  IPv6Endpoint dest_;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_MESSAGEINFO_H_
