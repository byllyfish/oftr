#include "ofp/featuresrequest.h"
#include "ofp/channel.h"
#include "ofp/log.h"


void ofp::FeaturesRequestBuilder::send(Channel *channel)
{
	log::debug(__PRETTY_FUNCTION__);

	UInt32 xid = channel->nextXid();

	msg_.header_.setLength(sizeof(FeaturesRequest));
	msg_.header_.setXid(xid);

	channel->write(&msg_, sizeof(msg_));
	channel->flush();
}