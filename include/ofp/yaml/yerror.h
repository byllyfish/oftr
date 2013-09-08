#ifndef OFP_YAML_YERROR_H
#define OFP_YAML_YERROR_H

#include "ofp/error.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Error> {

    static void mapping(IO &io, ofp::Error &msg)
    {
        ofp::UInt16 type = msg.errorType();
        ofp::UInt16 code = msg.errorCode();
        ofp::ByteRange data = msg.errorData();
        io.mapRequired("type", type);
        io.mapRequired("code", code);
        io.mapRequired("data", data);
    }
};


template <>
struct MappingTraits<ofp::ErrorBuilder> {

    static void mapping(IO &io, ofp::ErrorBuilder &msg)
    {
        ofp::UInt16 type;
        ofp::UInt16 code;
        ofp::ByteList data;
        io.mapRequired("type", type);
        io.mapRequired("code", code);
        io.mapRequired("data", data);
        msg.setErrorType(type);
        msg.setErrorCode(code);
        msg.setErrorData(data.data(), data.size());
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YERROR_H
