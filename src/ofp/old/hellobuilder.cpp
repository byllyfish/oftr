#include "ofp/hellobuilder.h"

	ofp::UInt32 ofp::HelloBuilder::send(Channel *channel)
	{
		UInt32 xid = channel->nextXid();

		Header &hdr = msg_.header_;
		hdr.setLength(UInt16_narrow_cast(msgLen));
		hdr.setXid(xid);

		elem_.type = OFPHET_VERSIONBITMAP;
		elem_.length = sizeof(elem_) + sizeof(bitmap_);
		bitmap_ = versions_.versionBitMap();

		channel->send(this, sizeof(HelloBuilder));

		return xid;
	}