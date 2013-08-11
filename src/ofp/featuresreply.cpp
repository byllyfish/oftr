#include "ofp/featuresreply.h"


const ofp::FeaturesReply *ofp::FeaturesReply::cast(const Message *message)
{
    assert(message->type() == OFPT_FEATURES_REPLY);

    const FeaturesReply *msg = reinterpret_cast<const FeaturesReply *>(message->data());
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
}


bool ofp::FeaturesReply::validateLength(size_t length) const
{
	return length == sizeof(FeaturesReply);
}