#ifndef OFP_PACKETIN_H
#define OFP_PACKETIN_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class PacketIn {
public:
	enum { Type = OFPT_PACKET_IN };

	static const PacketIn *cast(const Message *message);

	PacketIn() : header_{Type} {}

	UInt32 bufferID() const;
	UInt16 totalLen() const;
	UInt8 reason() const;
	UInt8 tableID() const;
	UInt64 cookie() const;

	Match match() const;
	ByteRange enetFrame() const;

private:
	Header header_;
	Big32 bufferID_;
	Big16 totalLen_;
	Big8 reason_;
	Big8 tableID_;
	Big64 cookie_;

	Big16 matchType_ = 0;
    Big16 matchLength_ = 0;
    Padding<4> pad_2;

	enum { UnpaddedSizeWithMatchHeader = 28 };
    enum { SizeWithoutMatchHeader = 24 };

	bool validateLength(size_t length) const;

	friend class PacketInBuilder;
};

static_assert(sizeof(PacketIn) == 32, "Unexpected size.");
static_assert(IsStandardLayout<PacketIn>(), "Expected standard layout.");

class PacketInBuilder {
public:
	PacketInBuilder() = default;

	void setBufferID(UInt32 bufferID);
	void setTotalLen(UInt16 totalLen);
	void setReason(UInt8 reason);
	void setTableID(UInt8 tableID);
	void setCookie(UInt64 cookie);

	void setMatch(const MatchBuilder &match) {
        match_ = match;
    }

    void setEnetFrame(const ByteRange &enetFrame) {
    	enetFrame_ = enetFrame;
    }

	UInt32 send(Writable *channel) {
		UInt8  version = channel->version();
        if (version <= 0x02) {
            return sendStandard(channel);
        }

        // Calculate length of PacketIn message up to end of match section. Then 
        // pad it to a multiple of 8 bytes.
        size_t msgMatchLen = PacketIn::UnpaddedSizeWithMatchHeader + match_.size();
        size_t msgMatchLenPadded = PadLength(msgMatchLen);

        // Calculate length of ethernet frame section (preceded by 2 byte pad.)
        size_t enetFrameLen = enetFrame_.size() + 2;

        // Calculate the total PacketIn message length.
        size_t msgLen = msgMatchLenPadded + enetFrameLen;

        // Fill in the message header.
        UInt32 xid = channel->nextXid();
        Header &hdr = msg_.header_;
        hdr.setVersion(version);
        hdr.setType(PacketIn::Type);
        hdr.setLength(UInt16_narrow_cast(msgLen));
        hdr.setXid(xid);

        // Fill in the match header.
        msg_.matchType_ = OFPMT_OXM;
        msg_.matchLength_ = UInt16_narrow_cast(PadLength(match_.size()));

        // Write the message with padding in the correct spots.
        Padding<8> pad;
        channel->write(&msg_, PacketIn::UnpaddedSizeWithMatchHeader);
        channel->write(match_.data(), match_.size());
        channel->write(&pad, msgMatchLenPadded - msgMatchLen);
        channel->write(&pad, 2);
        channel->write(enetFrame_.data(), enetFrame_.size());
        channel->flush();

        return xid;
	}

	UInt32 sendStandard(Writable *channel) 
	{
        UInt8 version = channel->version();
        assert(version <= 0x02);

        deprecated::StandardMatch stdMatch{match_.toRange()};

        size_t msgMatchLen = PacketIn::SizeWithoutMatchHeader + sizeof(stdMatch);
        size_t enetFrameLen = enetFrame_.size() + 2;
        size_t msgLen = msgMatchLen + enetFrameLen;

        UInt32 xid = channel->nextXid();
        Header &hdr = msg_.header_;
        hdr.setVersion(version);
        hdr.setType(PacketIn::Type);
        hdr.setLength(UInt16_narrow_cast(msgLen));
        hdr.setXid(xid);

        Padding<8> pad;
        channel->write(&msg_, PacketIn::SizeWithoutMatchHeader);
        channel->write(&stdMatch, sizeof(stdMatch));
        channel->write(&pad, 2);
        channel->write(enetFrame_.data(), enetFrame_.size());
        channel->flush();

        return xid;
	}

private:
	PacketIn msg_;
	MatchBuilder match_;
	ByteRange enetFrame_;
};

} // </namespace ofp>


#endif // OFP_PACKETIN_H
