#ifndef OFP_YAML_YHELLO_H
#define OFP_YAML_YHELLO_H

#include "ofp/hello.h"

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

template <class Type>
struct MessageWrap {
	MessageWrap(const Type *m) : msg{const_cast<Type *>(m)} {}
	Type *msg;
};

} // </namespace detail>
} // </namespace ofp>


namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <class Type>
struct MappingTraits<ofp::detail::MessageWrap<Type>> {
    static void mapping(IO &io, ofp::detail::MessageWrap<Type> &msg)
    {
        ofp::detail::readHeader(io, reinterpret_cast<ofp::Header *>(msg.msg));
        io.mapRequired("msg", *msg.msg);
    }
};

#if 0

template <>
struct SequenceTraits<ofp::Hello> {

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
struct MappingTraits<ofp::HelloBuilder> {

    static void mapping(IO &io, ofp::HelloBuilder &msg)
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

#endif //0

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YHELLO_H
