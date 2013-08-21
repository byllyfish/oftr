
#include "ofp/yaml/llvm_yaml.h"
#include "ofp/yaml/byteorder.h"
#include "ofp/yaml/flowmod.h"
#include "ofp/match.h"

using namespace ofp;


static void readHeader(llvm::yaml::IO &io, Header *header)
{
    UInt8 type;
    UInt8 version;
    UInt32 xid;

    io.mapRequired("type", type);
    io.mapRequired("xid", xid);
    io.mapRequired("version", version); 

    header->setType(type);
    header->setVersion(version);
    header->setXid(xid); 
}

static void writeHeader(llvm::yaml::IO &io, const Header *header)
{
    UInt8 version = header->version();
    UInt8 type = header->type();
    UInt32 xid = header->xid();

    io.mapRequired("type", type);
    io.mapRequired("version", version);
    io.mapRequired("xid", xid);
}


struct FlowModWrap {
    FlowModWrap(const ofp::FlowMod *m) : msg{const_cast<FlowMod *>(m)}
    {
    }

    ofp::FlowMod *msg;
};

template <>
struct llvm::yaml::MappingTraits<FlowModWrap> {

    static void mapping(llvm::yaml::IO &io, FlowModWrap &msg)
    {
        writeHeader(io, reinterpret_cast<Header *>(msg.msg));
        io.mapRequired("msg", *msg.msg);
    }
};

template <>
struct llvm::yaml::MappingTraits<FlowMod> {

    static void mapping(llvm::yaml::IO &io, FlowMod &msg)
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
        return match.itemCount();
    }

    static ofp::OXMIterator::Item &element(IO &io, ofp::Match &match,
                                           size_t index)
    {
        ofp::log::debug("match yaml item", index);

        ofp::OXMIterator iter = match.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return YamlRemoveConst_cast(*iter);
    }
};


class OXMItemReader {
public:
    OXMItemReader(llvm::yaml::IO &io, OXMIterator::Item &item, OXMType type) : io_{io}, item_{item}, type_{type} {}

    template <class ValueType>
    void visit() {
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


template <>
struct llvm::yaml::MappingTraits<ofp::OXMIterator::Item> {

    static void mapping(llvm::yaml::IO &io, ofp::OXMIterator::Item &item)
    {
        ofp::OXMType type = item.type();
        io.mapRequired("type", type);

        OXMItemReader reader{io, item, type};
        OXMInternalID id = type.internalID();
        OXMDispatch(id, &reader);
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
        if (!value.parse(scalar)) {
            return "Invalid OXM type.";
        }
        
        return "";
    }
};

template <>
struct llvm::yaml::ScalarTraits<ofp::IPv4Address> {
    static void output(const ofp::IPv4Address &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::IPv4Address &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid IPv4 address.";
        }
        return "";
    }
};

template <>
struct llvm::yaml::ScalarTraits<ofp::IPv6Address> {
    static void output(const ofp::IPv6Address &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::IPv6Address &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid IPv6 address.";
        }
        return "";
    }
};


template <>
struct llvm::yaml::ScalarTraits<ofp::EnetAddress> {
    static void output(const ofp::EnetAddress &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::EnetAddress &value)
    {
        if (!value.parse(scalar)) {
            return "Invalid Ethernet address.";
        }
        return "";
    }
};

struct FlowModBuilderWrap {
    FlowModBuilderWrap(ofp::FlowModBuilder *m) : msg{m} {}
    ofp::FlowModBuilder *msg;
};

template <>
struct llvm::yaml::MappingTraits<FlowModBuilderWrap> {

    static void mapping(llvm::yaml::IO &io, FlowModBuilderWrap &msg)
    {
        readHeader(io, reinterpret_cast<Header *>(msg.msg));
        io.mapRequired("msg", *msg.msg);
    }
};


template <>
struct llvm::yaml::MappingTraits<FlowModBuilder> {

    static void mapping(llvm::yaml::IO &io, FlowModBuilder &msg)
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
    }
};


struct MatchBuilderItem 
{
};

template <>
struct llvm::yaml::SequenceTraits<ofp::MatchBuilder> {

    static size_t size(IO &io, ofp::MatchBuilder &match)
    {
        return match.itemCount();
    }

    static MatchBuilderItem &element(IO &io, ofp::MatchBuilder &match,
                                           size_t index)
    {
        return reinterpret_cast<MatchBuilderItem&>(match);
    }
};


class MatchBuilderInserter {
public:
    MatchBuilderInserter(llvm::yaml::IO &io, MatchBuilder &builder, OXMType type) : io_{io}, builder_{builder}, type_{type} {}

    template <class ValueType>
    void visit() {
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


template <>
struct llvm::yaml::MappingTraits<MatchBuilderItem> {

    static void mapping(llvm::yaml::IO &io, MatchBuilderItem &item)
    {
        MatchBuilder &builder = reinterpret_cast<MatchBuilder &>(item);

        ofp::OXMType type;
        io.mapRequired("type", type);

        OXMInternalID id = type.internalID();
        if (id != OXMInternalID::UNKNOWN) {
            MatchBuilderInserter inserter{io, builder, type};
            OXMDispatch(id, &inserter);

        } else {
            log::debug("Unknown oxmtype: ", type);
        }
    }
};



namespace ofp {  // <namespace ofp>
namespace yaml { // <namespace yaml>

template <>
Exception read(const std::string &input, FlowModBuilder *msg)
{
    llvm::yaml::Input yin(input);

    FlowModBuilderWrap wrap{msg};
    yin >> wrap;

    if (yin.error()) {
        return Exception{}; //FIXME
    }

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
std::string write(const FlowMod *msg)
{
    std::string result;
    llvm::raw_string_ostream rss{result};
    llvm::yaml::Output yout{rss};

    FlowModWrap wrap{msg};
    yout << wrap;
    return rss.str();
}

} // </namespace yaml>
} // </namespace ofp>