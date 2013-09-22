#ifndef OFP_YAML_YMULTIPARTREPLY_H
#define OFP_YAML_YMULTIPARTREPLY_H

#include "ofp/multipartreply.h"
#include "ofp/yaml/yflowstatsreply.h"


namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>


/// Elements are variable size. First two bytes of an element contain the
/// element size.
template <class Type>
class MPReplyVariableSizeSeq {
public:
    MPReplyVariableSizeSeq(MultipartReply &msg) : msg_(msg), position_{msg.replyBody()} {}

    size_t size() const {
        size_t result = 0;
        size_t total = msg_.replyBodySize();
        const UInt8 *buf = msg_.replyBody();
        // FIXME check alignment
        size_t len = 0;
        while (len < total) {
            UInt16 elemSize = *reinterpret_cast<const Big16 *>(buf + len);
            len += elemSize;
            ++result;
        }
        return result;
    }

    Type &next() {
        const UInt8 *pos = position_;
        position_ += *reinterpret_cast<const Big16 *>(position_);
        return RemoveConst_cast(*reinterpret_cast<const Type *>(pos));
    }

private:
    MultipartReply &msg_;
    const UInt8 *position_;
};

OFP_BEGIN_IGNORE_PADDING

template <class Type>
class MPReplyBuilderSeq {
public:
    MPReplyBuilderSeq() : init_{false} {}

    Type &next() {
        if (init_) {
            item_.write(&channel_);
        } else {
            init_ = true;
        }
        return item_;
    }

    void close() { (void) next(); }

    const UInt8 *data() const { return channel_.data(); }
    size_t size() const { return channel_.size(); }

private:
    MemoryChannel channel_;
    Type item_;
    bool init_;
};

OFP_END_IGNORE_PADDING

} // </namespace detail>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyVariableSizeSeq<Type>> {

    static size_t size(IO &io, ofp::detail::MPReplyVariableSizeSeq<Type> &seq)
    {
        return seq.size();
    }

    static Type &element(IO &io, ofp::detail::MPReplyVariableSizeSeq<Type> &seq,
                                     size_t index)
    {
        return seq.next();
    }    
};

template <class Type>
struct SequenceTraits<ofp::detail::MPReplyBuilderSeq<Type>> {

    static size_t size(IO &io, ofp::detail::MPReplyBuilderSeq<Type> &seq)
    {
        return 0;
    }

    static Type &element(IO &io, ofp::detail::MPReplyBuilderSeq<Type> &seq,
                                     size_t index)
    {
        return seq.next();
    }
};


template <>
struct MappingTraits<ofp::MultipartReply> {

    static void mapping(IO &io, ofp::MultipartReply &msg)
    {
        using namespace ofp;

        OFPMultipartType type = msg.replyType();
        io.mapRequired("type", type);
        io.mapRequired("flags", msg.flags_);

        switch (type) {
            case OFPMP_FLOW:{
                ofp::detail::MPReplyVariableSizeSeq<FlowStatsReply> seq{msg};
                io.mapRequired("body", seq);
                break;
            }
            case OFPMP_PORT_DESC:{
                //io.mapOptional("body", EmptyRequest);
                break;
            }
            default:
                // FIXME
                log::info("MultiPartReply: MappingTraits not fully implemented.", int(type));
                break;
        }
    }
};


template <>
struct MappingTraits<ofp::MultipartReplyBuilder> {

    static void mapping(IO &io, ofp::MultipartReplyBuilder &msg)
    {
        using namespace ofp;

        OFPMultipartType type;
        io.mapRequired("type", type);
        io.mapRequired("flags", msg.msg_.flags_);
        msg.setReplyType(type);

        switch (type) {
            case OFPMP_FLOW:{
                ofp::detail::MPReplyBuilderSeq<FlowStatsReplyBuilder> seq;
                io.mapRequired("body", seq);
                seq.close();
                msg.setReplyBody(seq.data(), seq.size());
                break;
            }
            case OFPMP_PORT_DESC:{
                //io.mapOptional("body", EmptyRequest);
                break;
            }
            default:
                // FIXME
                log::info("MultiPartReplyBuilder: MappingTraits not fully implemented.", int(type));
                break;
        };
    }
};

} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YMULTIPARTREPLY_H
