#ifndef OFP_YAML_YFEATURESREPLY_H
#define OFP_YAML_YFEATURESREPLY_H

#include "ofp/featuresreply.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ydatapathid.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::FeaturesReply> {

    static void mapping(IO &io, ofp::FeaturesReply &msg)
    {
    	using namespace ofp;

        DatapathID dpid = msg.datapathId();
        UInt32 bufferCount = msg.bufferCount();
        UInt8 tableCount = msg.tableCount();
        UInt8 auxiliaryId = msg.auxiliaryId();
        UInt32 capabilities = msg.capabilities();
        UInt32 reserved = msg.reserved();
        
        io.mapRequired("datapath_id", dpid);
        io.mapRequired("n_buffers", bufferCount);
        io.mapRequired("n_tables", tableCount);
        io.mapRequired("auxiliary_id", auxiliaryId);
        io.mapRequired("capabilities", capabilities);
        io.mapRequired("reserved", reserved);
    }
};

template <>
struct MappingTraits<ofp::FeaturesReplyBuilder> {

    static void mapping(IO &io, ofp::FeaturesReplyBuilder &msg)
    {
    	using namespace ofp;
    	
        DatapathID dpid;
        UInt32 bufferCount;
        UInt8 tableCount;
        UInt8 auxiliaryId;
        UInt32 capabilities;
        UInt32 reserved;
        
        io.mapRequired("datapath_id", dpid);
        io.mapRequired("n_buffers", bufferCount);
        io.mapRequired("n_tables", tableCount);
        io.mapRequired("auxiliary_id", auxiliaryId);
        io.mapRequired("capabilities", capabilities);
        io.mapRequired("reserved", reserved);

        msg.setDatapathId(dpid);
        msg.setBufferCount(bufferCount);
        msg.setTableCount(tableCount);
        msg.setAuxiliaryId(auxiliaryId);
        msg.setCapabilities(capabilities);
        msg.setReserved(reserved);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YFEATURESREPLY_H
