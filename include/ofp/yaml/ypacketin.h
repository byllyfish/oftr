// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPACKETIN_H_
#define OFP_YAML_YPACKETIN_H_

#include "ofp/packetin.h"

namespace llvm {
namespace yaml {

// type: OFPT_PACKET_IN
// msg:
//   buffer_id: <UInt32>            { Required }
//   total_len: <UInt16>            { Required }
//   in_port: <UInt32>              { Required }
//   in_phy_port: <UInt32>          { Required }
//   metadata: <UInt64>             { Required }
//   reason: <OFPPacketInReason>    { Required }
//   table_id: <UInt8>              { Required }
//   cookie: <UInt64>               { Required }
//   match: <Match>
//   data: <Bytes>                  { Required }

template <>
struct MappingTraits<ofp::PacketIn> {
  static void mapping(IO &io, ofp::PacketIn &msg) {
    using namespace ofp;

    // Remember that PacketIn uses cross-wired accessors.
    Hex32 bufferID = msg.bufferId();
    Hex16 totalLen = msg.totalLen();
    io.mapRequired("buffer_id", bufferID);
    io.mapRequired("total_len", totalLen);

    Hex32 inPort = msg.inPort();
    Hex32 inPhyPort = msg.inPhyPort();
    Hex64 metadata = msg.metadata();
    io.mapRequired("in_port", inPort);
    io.mapRequired("in_phy_port", inPhyPort);
    io.mapRequired("metadata", metadata);

    OFPPacketInReason reason = msg.reason();
    Hex8 tableID = msg.tableID();
    Hex64 cookie = msg.cookie();
    io.mapRequired("reason", reason);
    io.mapRequired("table_id", tableID);
    io.mapRequired("cookie", cookie);

    ofp::Match m = msg.match();
    io.mapRequired("match", m);

    ofp::ByteRange enetFrame = msg.enetFrame();
    io.mapRequired("data", enetFrame);
  }
};

template <>
struct MappingTraits<ofp::PacketInBuilder> {
  static void mapping(IO &io, ofp::PacketInBuilder &msg) {
    using namespace ofp;

    io.mapRequired("buffer_id", msg.msg_.bufferId_);
    io.mapRequired("total_len", msg.msg_.totalLen_);

    UInt32 inPort;
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

    ofp::yaml::Encoder *encoder =
        reinterpret_cast<ofp::yaml::Encoder *>(io.getContext());
    if (encoder && encoder->matchPrereqsChecked()) {
      if (!msg.match_.validate()) {
        // TODO(bfish) better error message
        io.setError("Match is ambiguous.");
        ofp::log::info("Match is ambiguous.");
      }
    }
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPACKETIN_H_
