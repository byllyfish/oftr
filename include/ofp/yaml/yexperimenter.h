#ifndef OFP_YAML_YEXPERIMENTER_H
#define OFP_YAML_YEXPERIMENTER_H

#include "ofp/experimenter.h"
#include "ofp/yaml/ybytelist.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::Experimenter> {

    static void mapping(IO &io, ofp::Experimenter &msg)
    {
        ofp::UInt32 experimenter = msg.experimenter();
        ofp::UInt32 expType = msg.expType();
        ofp::ByteRange data = msg.expData();
        io.mapRequired("experimenter", experimenter);
        io.mapRequired("exp_type", expType);
        io.mapRequired("data", data);
    }
};


template <>
struct MappingTraits<ofp::ExperimenterBuilder> {

    static void mapping(IO &io, ofp::ExperimenterBuilder &msg)
    {
        ofp::UInt32 experimenter;
        ofp::UInt32 expType;
        ofp::ByteList data;
        io.mapRequired("experimenter", experimenter);
        io.mapRequired("exp_type", expType);
        io.mapRequired("data", data);
        msg.setExperimenter(experimenter);
        msg.setExpType(expType);
        msg.setExpData(data.data(), data.size());
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YEXPERIMENTER_H
