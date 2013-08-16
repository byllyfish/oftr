#include "ofp/getconfigrequest.h"
#include "ofp/writable.h"


const ofp::GetConfigRequest *ofp::GetConfigRequest::cast(const Message *message)
{
    assert(message->type() == OFPT_GET_CONFIG_REQUEST);

    const GetConfigRequest *msg = reinterpret_cast<const GetConfigRequest *>(message->data());

    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;	
}


bool ofp::GetConfigRequest::validateLength(size_t length) const
{
	return length == sizeof(GetConfigRequest);
}


ofp::UInt32 ofp::GetConfigRequestBuilder::send(Writable *channel)
{
    UInt32 xid = channel->nextXid();

    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));
    msg_.header_.setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return xid;
}