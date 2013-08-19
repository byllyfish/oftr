
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
        ofp::log::debug("match yaml size");
        return match.size();
    }

    static ofp::OXMIterator::Item &element(IO &io, ofp::Match &match, size_t index)
    {
        ofp::log::debug("match yaml item", index);

        ofp::OXMIterator iter = match.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return YamlRemoveConst_cast(*iter);
    }
};


template <>
struct llvm::yaml::MappingTraits<ofp::OXMIterator::Item> {

    static void mapping(llvm::yaml::IO &io, ofp::OXMIterator::Item &item)
    {
        ofp::OXMType type = item.type();
        io.mapRequired("type", type);

        switch (type) 
        {
            case ofp::OFB_IN_PORT::type():
                auto val = item.value<ofp::OFB_IN_PORT>().value();
                io.mapRequired("value", val);
                break;
        }

        if (type.hasMask()) {
            //io.mapRequired("mask", item.mask<)
        }
    }
};


template <>
struct llvm::yaml::ScalarTraits<ofp::OXMType> {
    static void output(const ofp::OXMType &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        const ofp::OXMTypeInfo *info = value.lookupInfo();
        if (info) {
            out << info->name;
        } else {
            out << value.oxmNative();
        }
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::OXMType &value)
    {
        ofp::log::debug("OXMType scalar unimplemented");
        return StringRef{"huh?"};
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
//      ---
//      version: $header.version()
//      type: $header.type()
//      xid: $header.xid()
//      msg:
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
//      ...


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