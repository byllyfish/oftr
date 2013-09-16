#ifndef OFP_YAML_YFLOWMOD_H
#define OFP_YAML_YFLOWMOD_H

#include "ofp/yaml.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yinstructions.h"
#include "ofp/flowmod.h"
#include "ofp/match.h"

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

#if 0
template <class Type>
struct MessageWrap {
    MessageWrap(const Type *m) : msg{const_cast<Type *>(m)} {}
    Type *msg;
};

#endif

struct FlowModWrap {
    FlowModWrap(const ofp::FlowMod *m) : msg{const_cast<FlowMod *>(m)}
    {
    }
    ofp::FlowMod *msg;
};

struct FlowModBuilderWrap {
    FlowModBuilderWrap(ofp::FlowModBuilder *m) : msg{m}
    {
    }
    ofp::FlowModBuilder *msg;
};

#if 0
OFP_BEGIN_IGNORE_PADDING

class OXMItemReader {
public:
    OXMItemReader(llvm::yaml::IO &io, OXMIterator::Item &item, OXMType type)
        : io_(io), item_(item), type_{type}
    {
    }

    template <class ValueType>
    void visit()
    {
        auto val = item_.value<ValueType>().value();
        io_.mapRequired("value", val);
        if (type_.hasMask()) {
            auto mask = item_.mask<ValueType>().value();
            io_.mapRequired("mask", mask);
        }
    }

private:
    llvm::yaml::IO &io_;
    OXMIterator::Item &item_;
    OXMType type_;
};

class MatchBuilderInserter {
public:
    MatchBuilderInserter(llvm::yaml::IO &io, MatchBuilder &builder,
                         OXMType type)
        : io_(io), builder_(builder), type_{type}
    {
    }

    template <class ValueType>
    void visit()
    {
        if (type_.hasMask()) {
            typename ValueType::NativeType value;
            typename ValueType::NativeType mask;
            io_.mapRequired("value", value);
            io_.mapRequired("mask", mask);
            builder_.add(ValueType{value}, ValueType{mask});
        } else {
            typename ValueType::NativeType value;
            io_.mapRequired("value", value);
            builder_.add(ValueType{value});
        }
    }

private:
    llvm::yaml::IO &io_;
    MatchBuilder &builder_;
    OXMType type_;
};

OFP_END_IGNORE_PADDING

struct MatchBuilderItem {
};
#endif //0
#
} // </namespace detail>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

#if 0
template <>
struct MappingTraits<ofp::detail::FlowModWrap> {

    static void mapping(IO &io, ofp::detail::FlowModWrap &msg)
    {
        ofp::detail::writeHeader(io, reinterpret_cast<ofp::Header *>(msg.msg));
        io.mapRequired("msg", *msg.msg);
    }
};
#endif //0

template <>
struct MappingTraits<ofp::FlowMod> {

    static void mapping(IO &io, ofp::FlowMod &msg)
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

        ofp::InstructionRange instr = msg.instructions();
        io.mapRequired("instructions", instr);
    }
};

#if 0
template <>
struct SequenceTraits<ofp::Match> {

    static size_t size(IO &io, ofp::Match &match)
    {
        return match.itemCount();
    }

    static ofp::OXMIterator::Item &element(IO &io, ofp::Match &match,
                                           size_t index)
    {
        ofp::log::debug("match yaml item", index);

        // FIXME
        ofp::OXMIterator iter = match.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return RemoveConst_cast(*iter);
    }
};

template <>
struct MappingTraits<ofp::OXMIterator::Item> {

    static void mapping(IO &io, ofp::OXMIterator::Item &item)
    {
        ofp::OXMType type = item.type();
        io.mapRequired("type", type);

        ofp::detail::OXMItemReader reader{io, item, type};
        ofp::OXMInternalID id = type.internalID();
        OXMDispatch(id, &reader);
    }
};

template <>
struct ScalarTraits<ofp::OXMType> {
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
        if (!value.parse(scalar)) {
            return "Invalid OXM type.";
        }

        return "";
    }
};
#endif //0

#if 0
template <>
struct MappingTraits<ofp::detail::FlowModBuilderWrap> {

    static void mapping(IO &io, ofp::detail::FlowModBuilderWrap &msg)
    {
        ofp::detail::readHeader(io, reinterpret_cast<ofp::Header *>(msg.msg));
        io.mapRequired("msg", *msg.msg);
    }
};
#endif //0

template <>
struct MappingTraits<ofp::FlowModBuilder> {

    static void mapping(IO &io, ofp::FlowModBuilder &msg)
    {
        io.mapRequired("cookie", msg.msg_.cookie_);
        io.mapRequired("cookie_mask", msg.msg_.cookieMask_);
        io.mapRequired("table_id", msg.msg_.tableId_);
        io.mapRequired("command", msg.msg_.command_);
        io.mapRequired("idle_timeout", msg.msg_.idleTimeout_);
        io.mapRequired("hard_timeout", msg.msg_.hardTimeout_);
        io.mapRequired("priority", msg.msg_.priority_);
        io.mapRequired("buffer_id", msg.msg_.bufferId_);
        io.mapRequired("out_port", msg.msg_.outPort_);
        io.mapRequired("out_group", msg.msg_.outGroup_);
        io.mapRequired("flags", msg.msg_.flags_);

        io.mapRequired("match", msg.match_);
        io.mapRequired("instructions", msg.instructions_);
    }
};

#if 0
template <>
struct SequenceTraits<ofp::MatchBuilder> {

    static size_t size(IO &io, ofp::MatchBuilder &match)
    {
        return match.itemCount();
    }

    static ofp::detail::MatchBuilderItem &element(IO &io, ofp::MatchBuilder &match,
                                     size_t index)
    {
        return reinterpret_cast<ofp::detail::MatchBuilderItem &>(match);
    }
};

template <>
struct MappingTraits<ofp::detail::MatchBuilderItem> {

    static void mapping(IO &io, ofp::detail::MatchBuilderItem &item)
    {
        ofp::MatchBuilder &builder = reinterpret_cast<ofp::MatchBuilder &>(item);

        ofp::OXMType type;
        io.mapRequired("type", type);

        ofp::OXMInternalID id = type.internalID();
        if (id != ofp::OXMInternalID::UNKNOWN) {
            ofp::detail::MatchBuilderInserter inserter{io, builder, type};
            OXMDispatch(id, &inserter);

        } else {
            ofp::log::debug("Unknown oxmtype: ", type);
        }
    }
};
#endif //0

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YFLOWMOD_H
