// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPACKETIN_H_
#define OFP_YAML_YPACKETIN_H_

#include "ofp/packetin.h"
#include "ofp/yaml/ybufferid.h"
#include "ofp/yaml/ymatchpacket.h"

namespace llvm {
namespace yaml {

const char *const kPacketInSchema = R"""({Message/PacketIn}
type: 'PACKET_IN'
msg:
  buffer_id: BufferID
  total_len: UInt16
  in_port: PortNumber
  in_phy_port: UInt32
  metadata: UInt64
  reason: PacketInReason
  table_id: TableNumber
  cookie: UInt64
  match: [{Field}...]
  data: HexString
  _data_pkt: [{Field}...]        # Output only; Optional
)""";

template <>
struct MappingTraits<ofp::PacketIn> {
  static void mapping(IO &io, ofp::PacketIn &msg) {
    using namespace ofp;

    // Remember that PacketIn uses cross-wired accessors.
    BufferID bufferID = msg.bufferId();
    Hex16 totalLen = msg.totalLen();
    io.mapRequired("buffer_id", bufferID);
    io.mapRequired("total_len", totalLen);

    PortNumber inPort = msg.inPort();
    Hex32 inPhyPort = msg.inPhyPort();
    Hex64 metadata = msg.metadata();
    io.mapRequired("in_port", inPort);
    io.mapRequired("in_phy_port", inPhyPort);
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
      io.mapRequired("_data_pkt", mp);
    }
  }
};

template <>
struct MappingTraits<ofp::PacketInBuilder> {
  static void mapping(IO &io, ofp::PacketInBuilder &msg) {
    using namespace ofp;

    BufferID bufferId;
    io.mapRequired("buffer_id", bufferId);
    msg.setBufferId(bufferId);

    io.mapRequired("total_len", msg.msg_.totalLen_);

    PortNumber inPort;
    UInt32 inPhyPort;
    UInt64 metadata;
    io.mapRequired("in_port", inPort);
    io.mapRequired("in_phy_port", inPhyPort);
    io.mapRequired("metadata", metadata);
    msg.setInPort(inPort);
    msg.setInPhyPort(inPhyPort);
    msg.setMetadata(metadata);

    io.mapRequired("reason", msg.msg_.reason_);
    io.mapRequired("table_id", msg.msg_.tableID_);
    io.mapRequired("cookie", msg.msg_.cookie_);

    io.mapOptional("match", msg.match_);
    io.mapRequired("data", msg.enetFrame_);

    MatchBuilder ignoreDataMatch;  // FIXME(bfish) Add `mapIgnore` method?
    io.mapOptional("_data_pkt", ignoreDataMatch);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPACKETIN_H_
