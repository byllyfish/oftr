#include "ofp/controllerhandshake.h"
#include "ofp/internalchannel.h"
#include "ofp/hello.h"
#include "ofp/message.h"
#include "ofp/features.h"
#include "ofp/featuresrequest.h"
#include "ofp/featuresreply.h"
#include "ofp/error.h"
#include "ofp/log.h"
#include "ofp/constants.h"


ofp::ControllerHandshake::ControllerHandshake(InternalChannel *channel, ProtocolVersions versions, Factory listenerFactory)
	: channel_{channel}, versions_{versions}, listenerFactory_{listenerFactory} 
{
}

void ofp::ControllerHandshake::onChannelUp(Channel *channel) 
{
	log::debug(__PRETTY_FUNCTION__);

	assert(channel == channel_);

	// We have a brand new connection. Send a OFPT_HELLO message to mutually agree
	// on a version.
	
	HelloBuilder msg{versions_};
	msg.send(channel_);
}

void ofp::ControllerHandshake::onChannelDown() 
{
	log::debug(__PRETTY_FUNCTION__);

	log::info("Channel down before controller handshake could complete.");
}

void ofp::ControllerHandshake::onMessage(const Message *message) 
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

void ofp::ControllerHandshake::onException(const Exception *exception)
{
	log::debug(__PRETTY_FUNCTION__);

	// TODO
}


void ofp::ControllerHandshake::onHello(const Message *message)
{
	const Hello *msg = Hello::cast(message);
	if (!msg)
		return;

	ProtocolVersions versions = msg->protocolVersions().intersection(versions_);

	if (versions.empty()) {
		ErrorBuilder{OFPET_HELLO_FAILED, OFPHFC_INCOMPATIBLE}.send(channel_);
		channel_->close();
	} else {
		channel_->setProtocolVersion(versions.highestVersion());
		FeaturesRequestBuilder{}.send(channel_);
	}
}


void ofp::ControllerHandshake::onFeaturesReply(const Message *message)
{
	const FeaturesReply *msg = FeaturesReply::cast(message);
	if (!msg)
		return;

	Features features;
	msg->getFeatures(&features);
	channel_->setFeatures(features);

	// Install new ChannelListener.
	ChannelListener *newListener = listenerFactory_();
	assert(newListener);
	channel_->setChannelListener(newListener);
	newListener->onChannelUp(channel_);

	delete this;
}


void ofp::ControllerHandshake::onError(const Message *message) 
{
	// FIXME log it
}

