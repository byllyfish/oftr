#include "ofp/packetout.h"


void ofp::PacketOutBuilder::setBufferID(UInt32 bufferID)
{
	msg_.bufferID_ = bufferID;
}

void ofp::PacketOutBuilder::setInPort(UInt32 inPort)
{
	msg_.inPort_ = inPort;
}

void ofp::PacketOutBuilder::setActions(ActionRange actions)
{
	actions_ = actions;
}

void ofp::PacketOutBuilder::setEnetFrame(ByteRange enetFrame)
{
	enetFrame_ = enetFrame;
}

