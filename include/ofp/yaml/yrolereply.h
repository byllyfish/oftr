#ifndef OFP_YAML_YROLEREPLY_H
#define OFP_YAML_YROLEREPLY_H

#include "ofp/rolereply.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::RoleReply> {

    static void mapping(IO &io, ofp::RoleReply &msg)
    {
    	io.mapRequired("role", msg.role_);
    	io.mapRequired("generation_id", msg.generationId_);
    }
};

template <>
struct MappingTraits<ofp::RoleReplyBuilder> {

    static void mapping(IO &io, ofp::RoleReplyBuilder &msg)
    {
        io.mapRequired("role", msg.msg_.role_);
    	io.mapRequired("generation_id", msg.msg_.generationId_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YROLEREPLY_H
