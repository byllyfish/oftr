#ifndef OFP_YAML_YHELLO_H
#define OFP_YAML_YHELLO_H

#include "ofp/hello.h"
#include "ofp/protocolversions.h"

LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(ofp::UInt8);

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Hello> {

    static void mapping(IO &io, ofp::Hello &msg)
    {
        ofp::ProtocolVersions versions = msg.protocolVersions();
        if (!versions.empty()) {
            std::vector<ofp::UInt8> vers = versions.versions();
            io.mapRequired("versions", vers);
        }
    }
};


template <>
struct MappingTraits<ofp::HelloBuilder> {

    static void mapping(IO &io, ofp::HelloBuilder &msg)
    {
        std::vector<ofp::UInt8> versions;
        io.mapRequired("versions", versions);
        msg.setProtocolVersions(ofp::ProtocolVersions{versions});
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YHELLO_H
