#ifndef OFP_YAML_YPORTSTATUS_H
#define OFP_YAML_YPORTSTATUS_H

#include "ofp/portstatus.h"
#include "ofp/yaml/yport.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::PortStatus> {

    static void mapping(IO &io, ofp::PortStatus &msg)
    {
        io.mapRequired("reason", msg.reason_);
        io.mapRequired("port", msg.port_);
    }
};

template <>
struct MappingTraits<ofp::PortStatusBuilder> {

    static void mapping(IO &io, ofp::PortStatusBuilder &msg)
    {
        io.mapRequired("reason", msg.msg_.reason_);
        io.mapRequired("port", msg.msg_.port_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPORTSTATUS_H
