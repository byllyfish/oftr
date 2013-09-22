#ifndef OFP_YAML_MULTIPARTREQUEST_H
#define OFP_YAML_MULTIPARTREQUEST_H

#include "ofp/multipartrequest.h"
#include "ofp/flowstatsrequest.h"
#include "ofp/yaml/ymultipartbody.h"
#include "ofp/yaml/yflowstatsrequest.h"
#include "ofp/memorychannel.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::MultipartRequest> {

    static void mapping(IO &io, ofp::MultipartRequest &msg)
    {
        using namespace ofp;

        OFPMultipartType type = msg.requestType();
        io.mapRequired("type", type);
        io.mapRequired("flags", msg.flags_);

        switch (type) {
        case OFPMP_FLOW: {
            const FlowStatsRequest *stats = FlowStatsRequest::cast(&msg);
            if (stats) {
                io.mapRequired("body", RemoveConst_cast(*stats));
            }
            break;
        }

        case OFPMP_PORT_DESC: {
            // io.mapOptional("body", EmptyRequest);
            break;
        }

        default:
            // FIXME - implement the rest.
            log::debug("MultipartRequest MappingTraits not fully implemented.");
            break;
        }
    }
};

template <>
struct MappingTraits<ofp::MultipartRequestBuilder> {

    static void mapping(IO &io, ofp::MultipartRequestBuilder &msg)
    {
        using namespace ofp;

        OFPMultipartType type;
        io.mapRequired("type", type);
        io.mapRequired("flags", msg.msg_.flags_);
        msg.setRequestType(type);

        switch (type) {
        case OFPMP_FLOW: {
            FlowStatsRequestBuilder stats;
            io.mapRequired("body", stats);
            MemoryChannel channel;
            stats.write(&channel);
            msg.setRequestBody(channel.data(), channel.size());
            break;
        }
        case OFPMP_PORT_DESC:
            // io.mapOptional("body", EmptyRequest);
            break;
        default:
            // FIXME - implement the rest.
            log::debug("MultipartRequestBuilder MappingTraits not fully implemented.");
            break;
        }
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_MULTIPARTREQUEST_H
