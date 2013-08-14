#include "ofp/getconfigreply.h"
#include "ofp/writable.h"


const ofp::GetConfigReply *ofp::GetConfigReply::cast(const Message *message)
{
    assert(message->type() == OFPT_GET_CONFIG_REPLY);

    const GetConfigReply *msg = reinterpret_cast<const GetConfigReply *>(message->data());

    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;	
}

bool ofp::GetConfigReply::validateLength(size_t length) const
{
	return length == sizeof(GetConfigReply);
}

void ofp::GetConfigReplyBuilder::setFlags(UInt16 flags)
{
	msg_.flags_ = flags;
}

void ofp::GetConfigReplyBuilder::setMissSendLen(UInt16 missSendLen)
{
	msg_.missSendLen_ = missSendLen;
}

ofp::UInt32 ofp::GetConfigReplyBuilder::send(Writable *channel)
{
   UInt32 xid = channel->nextXid();

    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));
    msg_.header_.setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return xid;
}

