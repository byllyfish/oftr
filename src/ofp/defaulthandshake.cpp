#include "ofp/defaulthandshake.h"
#include "ofp/sys/connection.h"
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

using sys::Connection;

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

		case FeaturesRequest::Type:
			onFeaturesRequest(message);
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

	if (versions.empty()) {
		replyError(OFPET_HELLO_FAILED, OFPHFC_INCOMPATIBLE, message);

	} else if (role_ == Driver::Controller) {
		channel_->setVersion(versions.highestVersion());
		FeaturesRequestBuilder{}.send(channel_);
		
	} else {
		assert(role_ == Driver::Agent || role_ == Driver::Auxiliary);

		channel_->setVersion(versions.highestVersion());
		// Agent connections wait for FeaturesRequest message.
	}

	// TODO handle cast where we reconnected with a startingVersion of 1 but
	// the other end supports a higher version number.
}


void DefaultHandshake::onFeaturesRequest(const Message *message)
{
	// A controller shouuld not receive a features request message.
	if (role_ == Driver::Controller) {
		return; // FIXME error
	}

	const FeaturesRequest *request = FeaturesRequest::cast(message);
	if (!request)
		return;  // FIXME log

	FeaturesReplyBuilder reply{message};
	reply.setFeatures(channel_->features());
	reply.send(channel_);

	installNewChannelListener();
}


void DefaultHandshake::onFeaturesReply(const Message *message)
{
	// Only a controller should be receiving a features reply message.
	if (role_ != Driver::Controller) {
		return;
	}

	const FeaturesReply *msg = FeaturesReply::cast(message);
	if (!msg)
		return;  // FIXME log

	Features features;
	msg->getFeatures(&features);
	channel_->setFeatures(features);

	// Registering the connection allows us to attach auxiliary connections to 
	// their main connections.

	channel_->postDatapathID();

	installNewChannelListener();
}


void DefaultHandshake::onError(const Message *message) 
{
	// FIXME log it
}



void DefaultHandshake::replyError(UInt16 type, UInt16 code, const Message *message)
{
	ErrorBuilder error{type, code, message};
	error.send(channel_);
	channel_->shutdown();
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
