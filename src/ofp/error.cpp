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
	log::debug(__PRETTY_FUNCTION__);
	channel->write(&msg_, sizeof(msg_));
	channel->flush();
}
