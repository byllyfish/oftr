#ifndef OFP_GETCONFIGREQUEST_H
#define OFP_GETCONFIGREQUEST_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

class GetConfigRequest {
public:
	enum { Type = OFPT_GET_CONFIG_REQUEST };

	static const GetConfigRequest *cast(const Message *message);

	GetConfigRequest() : header_{Type} {}

private:
	Header header_;

	bool validateLength(size_t length) const;

	friend class GetConfigRequestBuilder;
};

static_assert(sizeof(GetConfigRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetConfigRequest>(), "Expected standard layout.");

class GetConfigRequestBuilder {
public:
	GetConfigRequestBuilder() = default;

	UInt32 send(Writable *channel);

private:
	GetConfigRequest msg_;
};

} // </namespace ofp>

#endif // OFP_GETCONFIGREQUEST_H
