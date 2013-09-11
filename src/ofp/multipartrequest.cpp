#include "ofp/multipartrequest.h"
#include "ofp/writable.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>


const MultipartRequest *MultipartRequest::cast(const Message *message)
{
	return message->cast<MultipartRequest>();
}


bool MultipartRequest::validateLength(size_t length) const
{
	// FIXME - this will need to cast and verify the lengths in the body.
	return length >= sizeof(MultipartRequest);
}


UInt32 MultipartRequestBuilder::send(Writable *channel)
{
	UInt8 version = channel->version();
	UInt32 xid = channel->nextXid();
	size_t msgLen = sizeof(msg_) + body_.size();

	msg_.header_.setVersion(version);
	msg_.header_.setXid(xid);
	msg_.header_.setLength(UInt16_narrow_cast(msgLen));

	channel->write(&msg_, sizeof(msg_));
	channel->write(body_.data(), body_.size());
	channel->flush();

	return xid;
}


} // </namespace ofp>