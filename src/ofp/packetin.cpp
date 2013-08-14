#include "ofp/packetin.h"


void ofp::PacketInBuilder::setBufferID(UInt32 bufferID)
{
	msg_.bufferID_ = bufferID;
}

void ofp::PacketInBuilder::setTotalLen(UInt16 totalLen)
{
	msg_.totalLen_ = totalLen;
}

void ofp::PacketInBuilder::setReason(UInt8 reason)
{
	msg_.reason_ = reason;
}

void ofp::PacketInBuilder::setTableID(UInt8 tableID)
{
	msg_.tableID_ = tableID;
}

void ofp::PacketInBuilder::setCookie(UInt64 cookie)
{
	msg_.cookie_ = cookie;
}
