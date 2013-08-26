#include "ofp/error.h"
#include "ofp/log.h"


ofp::ErrorBuilder::ErrorBuilder(UInt16 type, UInt16 code, const Message *message)
{
	// message is included because we might send part of it back someday.
	// 
	msg_.type_ = type;
	msg_.code_ = code;
}

void ofp::ErrorBuilder::send(Channel *channel)
{
	msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));
    msg_.header_.setXid(channel->nextXid());

	channel->write(&msg_, sizeof(msg_));
	channel->flush();
}
