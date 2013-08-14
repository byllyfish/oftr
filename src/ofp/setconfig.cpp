#include "ofp/setconfig.h"
#include "ofp/writable.h"


const ofp::SetConfig *ofp::SetConfig::cast(const Message *message)
{
	assert(message->type() == OFPT_SET_CONFIG);

	const SetConfig *msg = reinterpret_cast<const SetConfig *>(message->data());
    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;
}


bool ofp::SetConfig::validateLength(size_t length) const
{
	return (header_.length() == sizeof(SetConfig));
}


void ofp::SetConfigBuilder::setFlags(UInt16 flags)
{
	msg_.flags_ = flags;
}

void ofp::SetConfigBuilder::setMissSendLen(UInt16 missSendLen)
{
	msg_.missSendLen_ = missSendLen;
}

ofp::UInt32 ofp::SetConfigBuilder::send(Writable *channel)
{
	UInt32 xid = channel->nextXid();

    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));
    msg_.header_.setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return xid;
}