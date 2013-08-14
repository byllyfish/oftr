#ifndef OFP_PACKETOUT_H
#define OFP_PACKETOUT_H

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
	Big32 bufferID_;
	Big32 inPort_;
	Big16 actionsLen_;
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

	UInt32 send(Writable *channel);

private:
	PacketOut msg_;
	ActionRange actions_;
	ByteRange enetFrame_;
};

} // </namespace ofp>

#endif // OFP_PACKETOUT_H
