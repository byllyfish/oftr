#include "ofp/featuresrequest.h"
#include "ofp/channel.h"
#include "ofp/log.h"

/*  -----  FeaturesRequest  ------------------------------------------------  */

ofp::FeaturesRequest::FeaturesRequest() : header_{Type}
{
}

const ofp::FeaturesRequest *ofp::FeaturesRequest::cast(const Message *message)
{
    assert(message->type() == OFPT_FEATURES_REQUEST);

    const FeaturesRequest *msg =
        reinterpret_cast<const FeaturesRequest *>(message->data());

    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;
}

bool ofp::FeaturesRequest::validateLength(size_t length) const
{
    return length == sizeof(FeaturesRequest);
}

/*  ----  FeaturesRequestBuilder  ------------------------------------------  */

ofp::UInt32 ofp::FeaturesRequestBuilder::send(Writable *channel)
{
    UInt32 xid = channel->nextXid();

    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));
    msg_.header_.setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return xid;
}
