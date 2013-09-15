#include "ofp/yaml/apichannellistener.h"
#include "ofp/yaml/apiserver.h"

using namespace ofp::yaml;

void ApiChannelListener::onChannelUp(Channel *channel)
{
	channel_ = channel;
	server_->onChannelUp(channel_);
}

void ApiChannelListener::onChannelDown(Channel *channel)
{
	assert(channel == channel_);
	server_->onChannelDown(channel_);
}

void ApiChannelListener::onMessage(const Message *message)
{
	server_->onMessage(channel_, message);
}

void ApiChannelListener::onException(const Exception *exception)
{
	server_->onException(channel_, exception);
}

void ApiChannelListener::onTimer(UInt32 timerID)
{
	server_->onTimer(channel_, timerID);
}
