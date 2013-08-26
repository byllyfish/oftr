#include "ofp/defaulthandshake.h"
#include "ofp/connection.h"
#include "ofp/hello.h"
#include "ofp/message.h"
#include "ofp/features.h"
#include "ofp/headeronly.h"
#include "ofp/featuresreply.h"
#include "ofp/error.h"
#include "ofp/log.h"
#include "ofp/constants.h"
#include "ofp/exception.h"

namespace ofp { // <namespace ofp>


DefaultHandshake::DefaultHandshake(Connection *channel, Driver::Role role, ProtocolVersions versions, Factory listenerFactory)
	: channel_{channel}, versions_{versions}, listenerFactory_{listenerFactory}, role_{role}
{
	assert(listenerFactory_ != nullptr);
}

void DefaultHandshake::onChannelUp(Channel *channel) 
{
	log::debug(__PRETTY_FUNCTION__);

	assert(channel == channel_);

	channel->setStartingXid(startingXid_);

	ProtocolVersions useVersion = versions_;
	if (startingVersion_ == OFP_VERSION_1) {
		// If the starting version is set to 1, we'll use this version on the
		// reconnect.
		useVersion = ProtocolVersions{startingVersion_};
	}
	
	HelloBuilder msg{useVersion};
	msg.send(channel_);
}

void DefaultHandshake::onChannelDown(Channel *channel) 
{
	log::debug(__PRETTY_FUNCTION__);

	log::info("Channel down before controller handshake could complete.");
}

void DefaultHandshake::onMessage(const Message *message) 
{
	log::debug(__PRETTY_FUNCTION__);

	switch (message->type())
	{
		case Hello::Type:
			onHello(message);
			break;

		case FeaturesReply::Type:
			onFeaturesReply(message);
			break;

		case Error::Type:
			onError(message);
			break;
			
		default:
			break;
	}

}

void DefaultHandshake::onException(const Exception *exception)
{
	log::debug(__PRETTY_FUNCTION__);

	// TODO
}


void DefaultHandshake::onHello(const Message *message)
{
	const Hello *msg = Hello::cast(message);
	if (!msg)
		return;

	ProtocolVersions versions = msg->protocolVersions();
	versions = versions.intersection(versions_);
	log::debug("bitmap ", versions.bitmap());

	if (versions.empty()) {
		ErrorBuilder error{OFPET_HELLO_FAILED, OFPHFC_INCOMPATIBLE, message};
		error.send(channel_);
		channel_->close();

	} else if (role_ == Driver::Controller) {
		channel_->setVersion(versions.highestVersion());
		FeaturesRequestBuilder{}.send(channel_);
		
	} else {
		channel_->setVersion(versions.highestVersion());
		installNewChannelListener();
	}
}


void DefaultHandshake::onFeaturesReply(const Message *message)
{
	const FeaturesReply *msg = FeaturesReply::cast(message);
	if (!msg)
		return;

	Features features;
	msg->getFeatures(&features);
	channel_->setFeatures(features);

	installNewChannelListener();
}


void DefaultHandshake::onError(const Message *message) 
{
	// FIXME log it
}


void DefaultHandshake::installNewChannelListener()
{
	assert(channel_->channelListener() == this);
	assert(listenerFactory_ != nullptr);

	ChannelListener *newListener = listenerFactory_();
	channel_->setChannelListener(newListener);
	newListener->onChannelUp(channel_);
}


} // </namespace ofp>
