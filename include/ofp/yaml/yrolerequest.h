#ifndef OFP_YAML_YROLEREQUEST_H
#define OFP_YAML_YROLEREQUEST_H

#include "ofp/rolerequest.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::RoleRequest> {

    static void mapping(IO &io, ofp::RoleRequest &msg)
    {
    	io.mapRequired("role", msg.role_);
    	io.mapRequired("generation_id", msg.generationId_);
    }
};


template <>
struct MappingTraits<ofp::RoleRequestBuilder> {

    static void mapping(IO &io, ofp::RoleRequestBuilder &msg)
    {
        io.mapRequired("role", msg.msg_.role_);
    	io.mapRequired("generation_id", msg.msg_.generationId_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YROLEREQUEST_H
