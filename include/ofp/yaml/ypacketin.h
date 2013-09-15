#ifndef OFP_YAML_YPACKETIN_H
#define OFP_YAML_YPACKETIN_H

#include "ofp/packetin.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::PacketIn> {

    static void mapping(IO &io, ofp::PacketIn &msg)
    {
        using namespace ofp;

        // Remember that PacketIn uses cross-wired accessors.
        UInt32 bufferID = msg.bufferId();
        UInt16 totalLen = msg.totalLen();

        io.mapRequired("buffer_id", bufferID);
        io.mapRequired("total_len", totalLen);

        UInt32 inPort = msg.inPort();
    	UInt32 inPhyPort = msg.inPhyPort();
    	UInt64 metadata = msg.metadata();
        io.mapRequired("in_port", inPort);
        io.mapRequired("in_phy_port", inPhyPort);
        io.mapRequired("metadata", metadata);

        OFPPacketInReason reason = msg.reason();
        UInt8 tableID = msg.tableID();
        UInt64 cookie = msg.cookie();
        io.mapRequired("reason", reason);
        io.mapRequired("table_id", tableID);
        io.mapRequired("cookie", cookie);

        ofp::ByteRange enetFrame = msg.enetFrame();
        io.mapRequired("enet_frame", enetFrame);
    }
};


template <>
struct MappingTraits<ofp::PacketInBuilder> {

    static void mapping(IO &io, ofp::PacketInBuilder &msg)
    {
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

        io.mapRequired("enet_frame", msg.enetFrame_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPACKETIN_H
