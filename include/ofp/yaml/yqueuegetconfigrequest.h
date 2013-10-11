#ifndef OFP_YAML_YQUEUEGETCONFIGREQUEST_H
#define OFP_YAML_YQUEUEGETCONFIGREQUEST_H

#include "ofp/queuegetconfigrequest.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::QueueGetConfigRequest> {

    static void mapping(IO &io, ofp::QueueGetConfigRequest &msg)
    {
    	io.mapRequired("port", msg.port_);
    }
};

template <>
struct MappingTraits<ofp::QueueGetConfigRequestBuilder> {

    static void mapping(IO &io, ofp::QueueGetConfigRequestBuilder &msg)
    {
        io.mapRequired("port", msg.msg_.port_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YQUEUEGETCONFIGREQUEST_H
