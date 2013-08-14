#ifndef OFP_GETCONFIGREPLY_H
#define OFP_GETCONFIGREPLY_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

class GetConfigReply {
public:
	enum { Type = OFPT_GET_CONFIG_REPLY };

	static const GetConfigReply *cast(const Message *message);

	GetConfigReply() : header_{Type} {}

	UInt16 flags() const { return flags_; }
	UInt16 missSendLen() const { return missSendLen_; }

private:
	Header header_;
	Big16 flags_;
	Big16 missSendLen_;

	bool validateLength(size_t length) const;

	friend class GetConfigReplyBuilder;
};

static_assert(sizeof(GetConfigReply) == 12, "Unexpected size.");
static_assert(IsStandardLayout<GetConfigReply>(), "Expected standard layout.");

class GetConfigReplyBuilder {
public:
	GetConfigReplyBuilder() = default;

	void setFlags(UInt16 flags);
	void setMissSendLen(UInt16 missSendLen);

	UInt32 send(Writable *channel);

private:
	GetConfigReply msg_;
};

} // </namespace ofp>

#endif // OFP_GETCONFIGREPLY_H
