#ifndef OFP_GETASYNCREQUEST_H
#define OFP_GETASYNCREQUEST_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

class GetAsyncRequest {
public:
	enum { Type = OFPT_GET_ASYNC_REQUEST };

	static const GetAsyncRequest *cast(const Message *message);

	GetAsyncRequest() : header_{Type} {}

private:
	Header header_;

	bool validateLength(size_t length) const;

	friend class GetAsyncRequestBuilder;
};

static_assert(sizeof(GetAsyncRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncRequest>(), "Expected standard layout.");

class GetAsyncRequestBuilder {
public:
	GetAsyncRequestBuilder() = default;

	UInt32 send(Writable *channel);

private:
	GetAsyncRequest msg_;
};

} // </namespace ofp>

#endif // OFP_GETASYNCREQUEST_H
