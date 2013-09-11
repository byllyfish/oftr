#include "ofp/featuresreply.h"
#include "ofp/channel.h"
#include "ofp/log.h"

namespace ofp { // <namespace ofp>

const FeaturesReply *ofp::FeaturesReply::cast(const Message *message)
{
    assert(message->type() == OFPT_FEATURES_REPLY);

    const FeaturesReply *msg =
        reinterpret_cast<const FeaturesReply *>(message->data());
    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;
}

void FeaturesReply::getFeatures(Features *features) const
{
    features->setDatapathId(datapathId_);
    features->setBufferCount(bufferCount_);
    features->setTableCount(tableCount_);
    features->setAuxiliaryId(auxiliaryId_);
    features->setCapabilities(capabilities_);
    features->setReserved(reserved_);
}

bool FeaturesReply::validateLength(size_t length) const
{
    if (length < sizeof(FeaturesReply)) {
        log::info("FeatureReply validateLength failed.");
        return false;
    }

    // TODO check number of ports in reply if version < 4.

    return true;
}

FeaturesReplyBuilder::FeaturesReplyBuilder(UInt32 xid)
{
    // Set xid of reply to request's xid.
    msg_.header_.setXid(xid);
}

void FeaturesReplyBuilder::setFeatures(const Features &features)
{
    msg_.datapathId_ = features.datapathId();
    msg_.bufferCount_ = features.bufferCount();
    msg_.tableCount_ = features.tableCount();
    msg_.auxiliaryId_ = features.auxiliaryId();
    msg_.capabilities_ = features.capabilities();
    msg_.reserved_ = features.reserved();
}

UInt32 FeaturesReplyBuilder::send(Writable *channel)
{
    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return msg_.header_.xid();
}

} // </namespace ofp>
