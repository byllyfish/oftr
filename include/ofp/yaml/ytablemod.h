#ifndef OFP_YAML_YTABLEMOD_H
#define OFP_YAML_YTABLEMOD_H

#include "ofp/tablemod.h"

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<ofp::TableMod> {

    static void mapping(IO &io, ofp::TableMod &msg)
    {
        io.mapRequired("table_id", msg.tableId_);
        io.mapRequired("config", msg.config_);
    }
};

template <>
struct MappingTraits<ofp::TableModBuilder> {

    static void mapping(IO &io, ofp::TableModBuilder &msg)
    {
        io.mapRequired("table_id", msg.msg_.tableId_);
        io.mapRequired("config", msg.msg_.config_);
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YTABLEMOD_H
