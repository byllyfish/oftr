#ifndef OFP_YAML_YPORTMOD_H
#define OFP_YAML_YPORTMOD_H

#include "ofp/portmod.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::PortMod> {

    static void mapping(IO &io, ofp::PortMod &msg)
    {
        io.mapRequired("port_no", msg.portNo_);
        io.mapRequired("hw_addr", msg.hwAddr_);
        io.mapRequired("config", msg.config_);
        io.mapRequired("mask", msg.mask_);
        io.mapRequired("advertise", msg.advertise_);
    }
};

template <>
struct MappingTraits<ofp::PortModBuilder> {

    static void mapping(IO &io, ofp::PortModBuilder &msg)
    {
        io.mapRequired("port_no", msg.msg_.portNo_);
        io.mapRequired("hw_addr", msg.msg_.hwAddr_);
        io.mapRequired("config", msg.msg_.config_);
        io.mapRequired("mask", msg.msg_.mask_);
        io.mapRequired("advertise", msg.msg_.advertise_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPORTMOD_H