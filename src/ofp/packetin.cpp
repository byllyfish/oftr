#include "ofp/packetin.h"

namespace ofp { // <namespace ofp>


bool PacketIn::validateLength(size_t length) const
{
	if (length < UnpaddedSizeWithMatchHeader) {
		log::debug("PacketIn too small.");
		return false;
	}

   // Check the match length.
    UInt16 matchLen = matchLength_;
    if (length != UnpaddedSizeWithMatchHeader + matchLen) {
        log::debug("PacketIn has mismatched lengths.");
        return false;
    }

    return true;
}

void PacketInBuilder::setBufferID(UInt32 bufferID)
{
	msg_.bufferID_ = bufferID;
}

void PacketInBuilder::setTotalLen(UInt16 totalLen)
{
	msg_.totalLen_ = totalLen;
}

void PacketInBuilder::setReason(UInt8 reason)
{
	msg_.reason_ = reason;
}

void PacketInBuilder::setTableID(UInt8 tableID)
{
	msg_.tableID_ = tableID;
}

void PacketInBuilder::setCookie(UInt64 cookie)
{
	msg_.cookie_ = cookie;
}

} // </namespace ofp>



