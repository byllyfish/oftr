// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/filteractiongenericreply.h"
#include "ofp/channel.h"
#include "ofp/matchpacket.h"
#include "ofp/matchpacketbuilder.h"
#include "ofp/packetout.h"

using namespace ofp;
using namespace ofp::rpc;

enum : UInt8 {
  ICMPV4_TYPE_ECHO_REQUEST = 8,
  ICMPV4_TYPE_ECHO_REPLY = 0,
  ICMPV4_CODE_ECHO = 0,
};

bool FilterActionGenericReply::apply(ByteRange enetFrame, PortNumber inPort,
                                     UInt64 metadata, Message *message) {
  MatchPacket pkt{enetFrame};

  OXMRange oxm = pkt.toRange();
  if (oxm.exists<OFB_ICMPV4_TYPE>()) {
    return applyICMPv4(enetFrame, inPort, message, oxm);
  } else if (oxm.exists<OFB_ICMPV6_TYPE>()) {
    return applyICMPv6(enetFrame, inPort, message, oxm);
  }

  return false;
}

bool FilterActionGenericReply::applyICMPv4(ByteRange enetFrame,
                                           PortNumber inPort, Message *message,
                                           OXMRange oxm) {
  // Verify the ICMPv4 type and code for an echo request.
  UInt8 icmpType = oxm.get<OFB_ICMPV4_TYPE>();
  UInt8 icmpCode = oxm.get<OFB_ICMPV4_CODE>();

  if (icmpType != ICMPV4_TYPE_ECHO_REQUEST || icmpCode != ICMPV4_CODE_ECHO) {
    log_warning("FilterActionGenericReply:applyICMPv4: invalid icmp header");
    return false;
  }

  ByteList reply;
  if (!buildICMPv4(enetFrame, oxm, &reply)) {
    return false;
  }
  sendPacketOut(&reply, inPort, message);

  return true;
}

bool FilterActionGenericReply::buildICMPv4(ByteRange enetFrame, OXMRange oxm,
                                           ByteList *reply) {
  MacAddress ethSrc = oxm.get<OFB_ETH_DST>();
  MacAddress ethDst = oxm.get<OFB_ETH_SRC>();

  if (ethDst.isMulticast()) {
    log_warning("FilterActionGenericReply::buildICMPv4: invalid eth_dst:",
                ethDst);
    return false;
  }

  OXMList fields;
  fields.add(OFB_ETH_TYPE{DATALINK_IPV4});
  fields.add(OFB_ETH_DST{ethDst});
  fields.add(OFB_ETH_SRC{ethSrc});

  UInt16 vlan = oxm.get<OFB_VLAN_VID>();
  if (vlan) {
    fields.add(OFB_VLAN_VID{vlan});
  }

  fields.add(OFB_IP_PROTO{PROTOCOL_ICMP});
  fields.add(OFB_IPV4_SRC{oxm.get<OFB_IPV4_DST>()});
  fields.add(OFB_IPV4_DST{oxm.get<OFB_IPV4_SRC>()});
  fields.add(OFB_ICMPV4_TYPE{ICMPV4_TYPE_ECHO_REPLY});
  fields.add(OFB_ICMPV4_CODE{ICMPV4_CODE_ECHO});

  UInt16 offset = oxm.get<X_PKT_POS>();
  ByteRange payload;
  if (offset > 0) {
    payload = SafeByteRange(enetFrame.data(), enetFrame.size(), offset);
  }

  MatchPacketBuilder pktBuild{fields.toRange()};
  pktBuild.build(reply, payload);

  return true;
}

bool FilterActionGenericReply::applyICMPv6(ByteRange enetFrame,
                                           PortNumber inPort, Message *message,
                                           OXMRange oxm) {
  return false;
}

void FilterActionGenericReply::sendPacketOut(const ByteList *data,
                                             PortNumber outPort,
                                             Message *message) {
  // Set flag to indicate we replied.
  message->setMsgFlags(message->msgFlags() | OFP_REPLIED);

  ActionList actions;
  actions.add(AT_OUTPUT{outPort});

  // FIXME(bfish): Handle version 6.
  PacketOutBuilder packetOut;
  packetOut.setInPort(OFPP_CONTROLLER);
  packetOut.setActions(actions);
  packetOut.setEnetFrame(*data);
  packetOut.send(message->source());
}
