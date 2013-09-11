#ifndef OFP_YAML_YECHO_H
#define OFP_YAML_YECHO_H

#include "ofp/yaml/yllvm.h"
#include "ofp/echorequest.h"
#include "ofp/echoreply.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>


template <>
struct MappingTraits<ofp::EchoRequest> {

    static void mapping(IO &io, ofp::EchoRequest &msg)
    {
        ofp::ByteRange data = msg.echoData();
        io.mapRequired("data", data);
    }
};


template <>
struct MappingTraits<ofp::EchoRequestBuilder> {

    static void mapping(IO &io, ofp::EchoRequestBuilder &msg)
    {
        ofp::ByteList data;
        io.mapRequired("data", data);
        msg.setEchoData(data.data(), data.size());
    }
};


template <>
struct MappingTraits<ofp::EchoReply> {

    static void mapping(IO &io, ofp::EchoReply &msg)
    {
        ofp::ByteRange data = msg.echoData();
        io.mapRequired("data", data);
    }
};

template <>
struct MappingTraits<ofp::EchoReplyBuilder> {

    static void mapping(IO &io, ofp::EchoReplyBuilder &msg)
    {
        ofp::ByteList data;
        io.mapRequired("data", data);
        msg.setEchoData(data.data(), data.size());
    }
};


} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YECHO_H
