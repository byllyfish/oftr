#include "ofp/internalchannel.h"

ofp::InternalChannel::~InternalChannel()
{
	ChannelListener::dispose(listener_);
}


ofp::UInt8 ofp::InternalChannel::version() const
{
	return version_;
}

void ofp::InternalChannel::setVersion(UInt8 version)
{
	version_ = version;
}

const ofp::Features &ofp::InternalChannel::features() const 
{
	return features_;
}

void ofp::InternalChannel::setFeatures(const Features &features)
{
	features_ = features;
}