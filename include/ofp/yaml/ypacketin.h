// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPACKETIN_H_
#define OFP_YAML_YPACKETIN_H_

#include "ofp/matchpacketbuilder.h"
#include "ofp/packetin.h"
#include "ofp/yaml/ybuffernumber.h"
#include "ofp/yaml/ymatchpacket.h"

namespace llvm {
namespace yaml {

const char *const kPacketInSchema = R"""({Message/PacketIn}
type: PACKET_IN
msg:
  buffer_id: BufferNumber
  total_len: UInt16
  in_port: PortNumber
  in_phy_port: !opt UInt32            # default = in_port
  metadata: UInt64
  reason: PacketInReason
  table_id: TableNumber
  cookie: UInt64
  match: [Field]
  data: HexData
  _pkt: !optout [Field]
  _pkt_data: !opt HexData             # default = ''
)""";

template <>
struct MappingTraits<ofp::PacketIn> {
  static void mapping(IO &io, ofp::PacketIn &msg) {
    using namespace ofp;

    // Remember that PacketIn uses cross-wired accessors.
    BufferNumber bufferID = msg.bufferId();
    Hex16 totalLen = msg.totalLen();
    io.mapRequired("buffer_id", bufferID);
    io.mapRequired("total_len", totalLen);

    PortNumber inPort = msg.inPort();
    Hex32 inPhyPort = msg.inPhyPort();
    Hex64 metadata = msg.metadata();
    io.mapRequired("in_port", inPort);
    io.mapOptional("in_phy_port", inPhyPort, inPort);
    io.mapRequired("metadata", metadata);

    OFPPacketInReason reason = msg.reason();
    TableNumber tableID = msg.tableID();
    Hex64 cookie = msg.cookie();
    io.mapRequired("reason", reason);
    io.mapRequired("table_id", tableID);
    io.mapRequired("cookie", cookie);

    ofp::Match m = msg.match();
    io.mapRequired("match", m);

    ofp::ByteRange enetFrame = msg.enetFrame();
    io.mapRequired("data", enetFrame);

    if (ofp::yaml::GetIncludePktMatchFromContext(io)) {
      ofp::MatchPacket mp{enetFrame};
      io.mapRequired("_pkt", mp);
    }
  }
};

template <>
struct MappingTraits<ofp::PacketInBuilder> {
  static void mapping(IO &io, ofp::PacketInBuilder &msg) {
    using namespace ofp;

    BufferNumber bufferId;
    io.mapRequired("buffer_id", bufferId);
    msg.setBufferId(bufferId);

    PortNumber inPort;
    UInt32 inPhyPort = 0;
    UInt64 metadata = 0;
    io.mapRequired("in_port", inPort);
    io.mapOptional("in_phy_port", inPhyPort, inPort);
    io.mapRequired("metadata", metadata);
    msg.setInPort(inPort);
    msg.setInPhyPort(inPhyPort);
    msg.setMetadata(metadata);

    io.mapRequired("reason", msg.msg_.reason_);
    io.mapRequired("table_id", msg.msg_.tableID_);
    io.mapRequired("cookie", msg.msg_.cookie_);

    io.mapOptional("match", msg.match_);
    io.mapRequired("data", msg.enetFrame_);

    MatchBuilder pktDecode;
    io.mapOptional("_pkt", pktDecode);
    if (msg.enetFrame_.empty() && pktDecode.size() > 0) {
      ByteList pktData;
      io.mapOptional("_pkt_data", pktData);
      ofp::MatchPacketBuilder mp{pktDecode.toRange()};
      mp.build(&msg.enetFrame_, pktData.toRange());
      // Set total_len from enetFrame if total_len is not present.
      io.mapOptional("total_len", msg.msg_.totalLen_, msg.enetFrame_.size());
    } else {
      io.mapRequired("total_len", msg.msg_.totalLen_);
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPACKETIN_H_
