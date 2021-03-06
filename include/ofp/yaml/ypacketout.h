// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPACKETOUT_H_
#define OFP_YAML_YPACKETOUT_H_

#include "ofp/matchpacketbuilder.h"
#include "ofp/oxmfields.h"
#include "ofp/packetout.h"
#include "ofp/packetoutv6.h"

namespace llvm {
namespace yaml {

const char *const kPacketOutSchema = R"""({Message/PacketOut}
type: PACKET_OUT
msg:
  buffer_id: !opt BufferNumber        # default=NO_BUFFER
  in_port: !opt PortNumber            # default=CONTROLLER
  match: !optout [Field]              # default=[] version=6
  actions: !opt [Action]              # default=[]
  data: !opt HexData                  # default=''
  _pkt: !optout [Field]
  _pkt_data: !opt HexData             # default=''
)""";

template <>
struct MappingTraits<ofp::PacketOut> {
  static void mapping(IO &io, ofp::PacketOut &msg) {
    using namespace ofp;

    io.mapRequired("buffer_id", msg.bufferId_);
    io.mapRequired("in_port", msg.inPort_);

    ActionRange actions = msg.actions();
    io.mapRequired("actions", actions);

    ByteRange enetFrame = msg.enetFrame();
    io.mapRequired("data", enetFrame);

    if (ofp::yaml::GetIncludePktMatchFromContext(io)) {
      ofp::MatchPacket mp{enetFrame, false};
      io.mapRequired("_pkt", mp);
    }
  }
};

template <>
struct MappingTraits<ofp::PacketOutBuilder> {
  static void mapping(IO &io, ofp::PacketOutBuilder &msg) {
    using namespace ofp;

    io.mapOptional("buffer_id", msg.msg_.bufferId_, OFP_NO_BUFFER);
    io.mapOptional("in_port", msg.msg_.inPort_, OFPP_CONTROLLER);
    io.mapOptional("actions", msg.actions_);
    io.mapOptional("data", msg.enetFrame_);

    MatchBuilder pktDecode;
    io.mapOptional("_pkt", pktDecode);
    if (msg.enetFrame_.empty() && pktDecode.size() > 0) {
      ByteList pktData;
      io.mapOptional("_pkt_data", pktData);
      ofp::MatchPacketBuilder mp{pktDecode.toRange()};
      mp.build(&msg.enetFrame_, pktData.toRange());
    }
  }
};

// There is a different message type for PACKET_OUT in OpenFlow 1.5.

template <>
struct MappingTraits<ofp::PacketOutV6> {
  static void mapping(IO &io, ofp::PacketOutV6 &msg) {
    using namespace ofp;

    io.mapRequired("buffer_id", msg.bufferId_);

    Match match = msg.match();
    PortNumber inPort = match.toRange().get<OFB_IN_PORT>();

    io.mapRequired("in_port", inPort);
    io.mapRequired("match", match);

    ActionRange actions = msg.actions();
    io.mapRequired("actions", actions);

    ByteRange enetFrame = msg.enetFrame();
    io.mapRequired("data", enetFrame);

    if (ofp::yaml::GetIncludePktMatchFromContext(io)) {
      ofp::MatchPacket mp{enetFrame, false};
      io.mapRequired("_pkt", mp);
    }
  }
};

template <>
struct MappingTraits<ofp::PacketOutV6Builder> {
  static void mapping(IO &io, ofp::PacketOutV6Builder &msg) {
    using namespace ofp;

    io.mapOptional("buffer_id", msg.msg_.bufferId_, OFP_NO_BUFFER);

    PortNumber inPort;
    io.mapOptional("in_port", inPort, OFPP_CONTROLLER);
    io.mapOptional("match", msg.match_);

    // If `match` does not contain entry for OFB_IN_PORT, we need to add it
    // using the value from `in_port`.
    MatchBuilder &match = msg.match();
    if (!match.toRange().exists<OFB_IN_PORT>()) {
      match.add(OFB_IN_PORT{inPort});
    }

    io.mapOptional("actions", msg.actions_);
    io.mapOptional("data", msg.enetFrame_);

    MatchBuilder pktDecode;
    io.mapOptional("_pkt", pktDecode);
    if (msg.enetFrame_.empty() && pktDecode.size() > 0) {
      ByteList pktData;
      io.mapOptional("_pkt_data", pktData);
      ofp::MatchPacketBuilder mp{pktDecode.toRange()};
      mp.build(&msg.enetFrame_, pktData.toRange());
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPACKETOUT_H_
