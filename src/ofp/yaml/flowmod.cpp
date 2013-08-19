
#include "ofp/yaml/llvm_yaml.h"
#include "ofp/yaml/byteorder.h"
#include "ofp/yaml/flowmod.h"
#include "ofp/match.h"

template <>
struct llvm::yaml::MappingTraits<ofp::FlowMod> {

    static void mapping(llvm::yaml::IO &io, ofp::FlowMod &msg)
    {
        io.mapRequired("cookie", msg.cookie_);
        io.mapRequired("cookie_mask", msg.cookieMask_);
        io.mapRequired("table_id", msg.tableId_);
        io.mapRequired("command", msg.command_);
        io.mapRequired("idle_timeout", msg.idleTimeout_);
        io.mapRequired("hard_timeout", msg.hardTimeout_);
        io.mapRequired("priority", msg.priority_);
        io.mapRequired("buffer_id", msg.bufferId_);
        io.mapRequired("out_port", msg.outPort_);
        io.mapRequired("out_group", msg.outGroup_);
        io.mapRequired("flags", msg.flags_);

        ofp::Match m = msg.match();
        io.mapRequired("match", m);
    }
};

template <>
struct llvm::yaml::SequenceTraits<ofp::Match> {

    static size_t size(IO &io, ofp::Match &match)
    {
        return match.size();
    }

    static ofp::OXMIterator::Item &element(IO &io, ofp::Match &match, size_t index)
    {
        ofp::OXMIterator iter = match.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return *iter;  //FIXME
    }
};


template <>
struct llvm::yaml::MappingTraits<ofp::OXMIterator> {

    static void mapping(llvm::yaml::IO &io, ofp::OXMIterator &iter)
    {
        ofp::OXMType type = iter.type();
        io.mapRequired("type", type);
    }
};

namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

template <>
Exception read(const ByteRange &input, FlowModBuilder *msg)
{
    return Exception{};
}

//
// Encode a FlowMod message as follows:
//
//    msg:
//      version: $header.version()
//      type: $header.type()
//      xid: $header.xid()
//      body:
//          cookie: $cookie_
//          cookie_mask: $cookieMask_
//          table_id: $tableId_
//          command: $command_
//          idle_timeout: $idleTimeout_
//          hard_timeout: $hardTimeout_
//          priority: $priority_
//          buffer_id: $bufferId_
//          out_port: $outPort_
//          out_group: $outGroup_
//          flags: $flags_
//          match:
//              - type: OFB_IN_PORT
//                value: 5
//          instructions:
//              - type: GOTO_TABLE
//                value: 5
//              - type: WRITE_ACTIONS
//                  - type: AT_SET_NW_TTL
//                    value: 6
//                  - type: AT_OUTPUT
//                    value:
//                      - port: 2
//                      - maxlen: 70
//

template <class Type>
Type &YamlRemoveConst_cast(const Type &v)
{
    return const_cast<Type &>(v);
}

template <>
void write(const FlowMod *msg, ByteList *output)
{
    using llvm::yaml::Output;

    Output yout{llvm::outs()};

    //
    yout << YamlRemoveConst_cast(*msg);
}

} // </namespace yaml>
} // </namespace ofp>