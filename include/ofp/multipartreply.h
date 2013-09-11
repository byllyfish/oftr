#ifndef OFP_MULTIPARTREPLY_H
#define OFP_MULTIPARTREPLY_H

#include "ofp/header.h"
#include "ofp/padding.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

class Message;
class Writable;
class MultipartReplyBuilder;

class MultipartReply {
public:
	static constexpr OFPType type()
    {
        return OFPT_MULTIPART_REPLY;
    }

    static const MultipartReply *cast(const Message *message);

	MultipartReply() : header_{OFPT_MULTIPART_REPLY} {}

	OFPMultipartType replyType() const { return type_; }
	UInt16 replyFlags() const { return flags_; }
	const UInt8 *replyBody() const { return BytePtr(this) + sizeof(MultipartReply); }
	size_t replyBodySize() const { return header_.length() - sizeof(MultipartReply); }

	bool validateLength(size_t length) const;

private:
	Header header_;
	Big<OFPMultipartType> type_;
	Big16 flags_;
	Padding<4> pad_;

	friend class MultipartReplyBuilder;
	friend struct llvm::yaml::MappingTraits<MultipartReply>;
	friend struct llvm::yaml::MappingTraits<MultipartReplyBuilder>;
};

class MultipartReplyBuilder {
public:
	MultipartReplyBuilder() = default;

	void setReplyType(OFPMultipartType type) { msg_.type_ = type; }
	void setReplyFlags(UInt16 flags) { msg_.flags_ = flags; }
	void setReplyBody(const void *data, size_t length) { body_.set(data, length); }
	
	UInt32 send(Writable *channel);

private:
	MultipartReply msg_;
	ByteList body_;

	friend struct llvm::yaml::MappingTraits<MultipartReplyBuilder>;
};

} // </namespace ofp>

#endif // OFP_MULTIPARTREPLY_H
