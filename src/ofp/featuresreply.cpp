#include "ofp/featuresreply.h"
#include "ofp/channel.h"

/*  -----  FeaturesReply ---------------------------------------------------  */

ofp::FeaturesReply::FeaturesReply() : header_{Type}
{
}

const ofp::FeaturesReply *ofp::FeaturesReply::cast(const Message *message)
{
    assert(message->type() == OFPT_FEATURES_REPLY);

    const FeaturesReply *msg =
        reinterpret_cast<const FeaturesReply *>(message->data());
    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;
}

void ofp::FeaturesReply::getFeatures(Features *features) const
{
    features->setDatapathID(DatapathID{datapathID_});
    features->setBufferCount(bufferCount_);
    features->setTableCount(tableCount_);
    features->setAuxiliaryID(auxiliaryID_);
    features->setCapabilities(capabilities_);
    features->setReserved(reserved_);
}

bool ofp::FeaturesReply::validateLength(size_t length) const
{
    return length == sizeof(FeaturesReply);
}

/*  -----  FeaturesReplyBuilder --------------------------------------------  */

ofp::FeaturesReplyBuilder::FeaturesReplyBuilder(const Message *request)
{
    // Set xid of reply to request's xid.
    msg_.header_.setXid(request->xid());
}

void ofp::FeaturesReplyBuilder::setFeatures(const Features &features)
{
    msg_.datapathID_ = features.datapathID();
    msg_.bufferCount_ = features.bufferCount();
    msg_.tableCount_ = features.tableCount();
    msg_.auxiliaryID_ = features.auxiliaryID();
    msg_.capabilities_ = features.capabilities();
    msg_.reserved_ = features.reserved();
}

ofp::UInt32 ofp::FeaturesReplyBuilder::send(Writable *channel)
{
    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return msg_.header_.xid();
}
