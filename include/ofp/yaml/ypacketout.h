// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPACKETOUT_H_
#define OFP_YAML_YPACKETOUT_H_

#include "ofp/packetout.h"

namespace llvm {
namespace yaml {

const char *const kPacketOutSchema = R"""({Message/PacketOut}
type: PACKET_OUT
msg:
  buffer_id: !opt BufferNumber        # default=NO_BUFFER
  in_port: !opt PortNumber            # default=CONTROLLER
  actions: !opt [Action]              # default=[]
  data: !opt HexData                  # default=''
  _pkt_decode: !optout [Field]
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
      io.mapRequired("_pkt_decode", mp);
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

    MatchBuilder ignorePktDecode;  // FIXME(bfish) Add `mapIgnore` method?
    io.mapOptional("_pkt_decode", ignorePktDecode);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPACKETOUT_H_
