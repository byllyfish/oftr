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


ofp::DefaultHandshake::DefaultHandshake(Connection *channel, Driver::Role role, ProtocolVersions versions, Factory listenerFactory)
	: channel_{channel}, versions_{versions}, listenerFactory_{listenerFactory}, role_{role}
{
}

void ofp::DefaultHandshake::onChannelUp(Channel *channel) 
{
	log::debug(__PRETTY_FUNCTION__);

	assert(channel == channel_);

	// We have a brand new connection. Send a OFPT_HELLO message to mutually agree
	// on a version.
	
	HelloBuilder msg{versions_};
	msg.send(channel_);
}

void ofp::DefaultHandshake::onChannelDown(Channel *channel) 
{
	log::debug(__PRETTY_FUNCTION__);

	log::info("Channel down before controller handshake could complete.");
}

void ofp::DefaultHandshake::onMessage(const Message *message) 
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

void ofp::DefaultHandshake::onException(const Exception *exception)
{
	log::debug(__PRETTY_FUNCTION__);

	// TODO
}


void ofp::DefaultHandshake::onHello(const Message *message)
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


void ofp::DefaultHandshake::onFeaturesReply(const Message *message)
{
	const FeaturesReply *msg = FeaturesReply::cast(message);
	if (!msg)
		return;

	Features features;
	msg->getFeatures(&features);
	channel_->setFeatures(features);

	installNewChannelListener();
}


void ofp::DefaultHandshake::onError(const Message *message) 
{
	// FIXME log it
}


void ofp::DefaultHandshake::installNewChannelListener()
{
	assert(channel_->channelListener() == this);

	if (listenerFactory_) {
		ChannelListener *newListener = listenerFactory_();
		channel_->setChannelListener(newListener);
		newListener->onChannelUp(channel_);
	} else {
		channel_->setChannelListener(nullptr);
	}

	delete this;
}

