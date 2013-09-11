#ifndef OFP_YAML_YFLOWSTATSREQUEST_H
#define OFP_YAML_YFLOWSTATSREQUEST_H

#include "ofp/flowstatsrequest.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::FlowStatsRequest> {

    static void mapping(IO &io, ofp::FlowStatsRequest &msg)
    {
    	using namespace ofp;

        UInt8 tableId = msg.tableId();
        UInt32 outPort = msg.outPort();
        UInt32 outGroup = msg.outGroup();
        UInt64 cookie = msg.cookie();
        UInt64 cookieMask = msg.cookieMask();
    
        io.mapRequired("table_id", tableId);
        io.mapRequired("out_port", outPort);
        io.mapRequired("out_group", outGroup);
        io.mapRequired("cookie", cookie);
        io.mapRequired("cookie_mask", cookieMask);

        ofp::Match m = msg.match();
        io.mapRequired("match", m);
    }
};


template <>
struct MappingTraits<ofp::FlowStatsRequestBuilder> {

    static void mapping(IO &io, ofp::FlowStatsRequestBuilder &msg)
    {
        io.mapRequired("table_id", msg.msg_.tableId_);
        io.mapRequired("out_port", msg.msg_.outPort_);
        io.mapRequired("out_group", msg.msg_.outGroup_);
        io.mapRequired("cookie", msg.msg_.cookie_);
        io.mapRequired("cookie_mask", msg.msg_.cookieMask_);
        io.mapRequired("match", msg.match_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YFLOWSTATSREQUEST_H
