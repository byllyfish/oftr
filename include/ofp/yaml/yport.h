#ifndef OFP_YAML_YPORT_H
#define OFP_YAML_YPORT_H

#include "ofp/port.h"
#include "ofp/yaml/ysmallcstring.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Port> {

    static void mapping(IO &io, ofp::Port &msg)
    {
        io.mapRequired("port_no", msg.portNo_);
        io.mapRequired("hw_addr", msg.hwAddr_);
        io.mapRequired("name", msg.name_);
        io.mapRequired("config", msg.config_);
        io.mapRequired("state", msg.state_);
       	io.mapRequired("curr", msg.curr_);
       	io.mapRequired("advertised", msg.advertised_);
       	io.mapRequired("supported", msg.supported_);
       	io.mapRequired("peer", msg.peer_);
       	io.mapRequired("curr_speed", msg.currSpeed_);
       	io.mapRequired("max_speed", msg.maxSpeed_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YPORT_H
