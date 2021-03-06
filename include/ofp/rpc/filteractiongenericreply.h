// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_RPC_FILTERACTIONGENERICREPLY_H_
#define OFP_RPC_FILTERACTIONGENERICREPLY_H_

#include "ofp/macaddress.h"
#include "ofp/oxmrange.h"
#include "ofp/rpc/filteraction.h"

namespace ofp {
namespace rpc {

OFP_BEGIN_IGNORE_PADDING

class FilterActionGenericReply : public FilterAction {
 public:
  bool apply(ByteRange enetFrame, PortNumber inPort, UInt64 metadata,
             Message *message) override;

 private:
  bool applyICMPv4(ByteRange enetFrame, PortNumber inPort, Message *message,
                   OXMRange oxm);
  bool buildICMPv4(ByteRange enetFrame, OXMRange oxm, ByteList *reply);

  bool applyICMPv6(ByteRange enetFrame, PortNumber inPort, Message *message,
                   OXMRange oxm);

  void sendPacketOut(const ByteList *data, PortNumber outPort,
                     Message *message);
};

OFP_END_IGNORE_PADDING

}  // namespace rpc
}  // namespace ofp

#endif  // OFP_RPC_FILTERACTIONGENERICREPLY_H_
