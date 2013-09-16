#ifndef OFP_YAML_YSETCONFIG_H
#define OFP_YAML_YSETCONFIG_H

#include "ofp/yaml.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/setconfig.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::SetConfig> {

    static void mapping(IO &io, ofp::SetConfig &msg)
    {
        io.mapRequired("flags", msg.flags_);
        io.mapRequired("miss_send_len", msg.missSendLen_);
    }
};

template <>
struct MappingTraits<ofp::SetConfigBuilder> {

    static void mapping(IO &io, ofp::SetConfigBuilder &msg)
    {
        io.mapRequired("flags", msg.msg_.flags_);
        io.mapRequired("miss_send_len", msg.msg_.missSendLen_);
    }
};


} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YSETCONFIG_H
