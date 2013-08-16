#include "ofp/channellistener.h"
#include "ofp/log.h"
#include "ofp/exception.h"


ofp::ChannelListener::~ChannelListener()
{
}

void ofp::ChannelListener::onChannelUp(Channel *channel)
{
	log::debug("ChannelListener - onChannelUp ignored:", channel);
}


void ofp::ChannelListener::onChannelDown(Channel *channel)
{
	log::debug("ChannelListener - onChannelUp ignored:", channel);
}

void ofp::ChannelListener::onException(const Exception *exception) 
{
	log::debug("ChannelListener - onException ignored:", *exception);
}

void ofp::ChannelListener::onTimer(UInt32 timerID) 
{
	log::debug("ChannelListener - onTimer ignored:", timerID);
}

void ofp::ChannelListener::dispose(ChannelListener *listener)
{
	delete listener;
}
