#ifndef OFP_YAML_YHEADERONLY_H
#define OFP_YAML_YHEADERONLY_H

#include "ofp/headeronly.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::FeaturesRequest> {
    static void mapping(IO &io, ofp::FeaturesRequest &msg)
    {
    }
};


template <>
struct MappingTraits<ofp::FeaturesRequestBuilder> {
    static void mapping(IO &io, ofp::FeaturesRequestBuilder &msg)
    {
    }
};

template <>
struct MappingTraits<ofp::GetAsyncRequest> {
    static void mapping(IO &io, ofp::GetAsyncRequest &msg)
    {
    }
};


template <>
struct MappingTraits<ofp::GetAsyncRequestBuilder> {
    static void mapping(IO &io, ofp::GetAsyncRequestBuilder &msg)
    {
    }
};

template <>
struct MappingTraits<ofp::GetConfigRequest> {
    static void mapping(IO &io, ofp::GetConfigRequest &msg)
    {
    }
};

template <>
struct MappingTraits<ofp::GetConfigRequestBuilder> {
    static void mapping(IO &io, ofp::GetConfigRequestBuilder &msg)
    {
    }
};

template <>
struct MappingTraits<ofp::BarrierRequest> {
    static void mapping(IO &io, ofp::BarrierRequest &msg)
    {
    }
};

template <>
struct MappingTraits<ofp::BarrierRequestBuilder> {
    static void mapping(IO &io, ofp::BarrierRequestBuilder &msg)
    {
    }
};

template <>
struct MappingTraits<ofp::BarrierReply> {
    static void mapping(IO &io, ofp::BarrierReply &msg)
    {
    }
};

template <>
struct MappingTraits<ofp::BarrierReplyBuilder> {
    static void mapping(IO &io, ofp::BarrierReplyBuilder &msg)
    {
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YHEADERONLY_H
