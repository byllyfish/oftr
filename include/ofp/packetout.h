//  ===== ---- ofp/packetout.h -----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the PacketOut and PacketOutBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PACKETOUT_H
#define OFP_PACKETOUT_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/actionrange.h"
#include "ofp/padding.h"
#include "ofp/constants.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class PacketOut {
public:
	enum { Type = OFPT_PACKET_OUT };

	static const PacketOut *cast(const Message *message);

	PacketOut() : header_{Type} {}

	UInt32 bufferID() const;
	UInt32 inPort() const;

	ActionRange actions() const;
	ByteRange enetFrame() const;

private:
	Header header_;
	Big32 bufferID_ = OFP_NO_BUFFER;
	Big32 inPort_ = 0;
	Big16 actionsLen_ = 0;
	Padding<6> pad_;

	bool validateLength(size_t length) const;

	friend class PacketOutBuilder;
};

static_assert(sizeof(PacketOut) == 24, "Unexpected size.");
static_assert(IsStandardLayout<PacketOut>(), "Expected standard layout.");

class PacketOutBuilder {
public:
	PacketOutBuilder() = default;

	void setBufferID(UInt32 bufferID);
	void setInPort(UInt32 inPort);
	void setActions(ActionRange actions);
	void setEnetFrame(ByteRange enetFrame);

	UInt32 send(Writable *channel)
	{
		UInt8  version = channel->version();

		size_t msgLen = sizeof(msg_) + actions_.size();
		if (msg_.bufferID_ != OFP_NO_BUFFER) {
			msgLen += enetFrame_.size();
		}
		
        // Fill in the message header.
        UInt32 xid = channel->nextXid();
        Header &hdr = msg_.header_;
        hdr.setVersion(version);
        hdr.setType(PacketOut::Type);
        hdr.setLength(UInt16_narrow_cast(msgLen));
        hdr.setXid(xid);

        // Fill in length of actions section.
        msg_.actionsLen_ = UInt16_narrow_cast(actions_.size());

        channel->write(&msg_, sizeof(msg_));
        channel->write(actions_.data(), actions_.size());
        if (msg_.bufferID_ != OFP_NO_BUFFER) {
        	channel->write(enetFrame_.data(), enetFrame_.size());
        }
        channel->flush();

        return xid;
	}

private:
	PacketOut msg_;
	ActionRange actions_;
	ByteRange enetFrame_;
};

} // </namespace ofp>

#endif // OFP_PACKETOUT_H
