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
#include "ofp/actionlist.h"
#include "ofp/padding.h"
#include "ofp/constants.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

class Message;
class PacketOutBuilder;

class PacketOut {
public:
	static constexpr OFPType type() { return OFPT_PACKET_OUT; }

	static const PacketOut *cast(const Message *message);

	PacketOut() : header_{type()} {}

	UInt32 bufferId() const { return bufferId_; }
	UInt32 inPort() const { return inPort_; }

	ActionRange actions() const;
	ByteRange enetFrame() const;

	bool validateLength(size_t length) const;
	
private:
	Header header_;
	Big32 bufferId_ = OFP_NO_BUFFER;
	Big32 inPort_ = 0;
	Big16 actionsLen_ = 0;
	Padding<6> pad_;

	friend class PacketOutBuilder;
	friend struct llvm::yaml::MappingTraits<PacketOutBuilder>;
};

static_assert(sizeof(PacketOut) == 24, "Unexpected size.");
static_assert(IsStandardLayout<PacketOut>(), "Expected standard layout.");

class PacketOutBuilder {
public:
	PacketOutBuilder() = default;

	void setBufferId(UInt32 bufferId) { msg_.bufferId_ = bufferId; }
	void setInPort(UInt32 inPort) { msg_.inPort_ = inPort; }
	void setActions(const ActionList &actions) {actions_ = actions;}
	void setEnetFrame(const ByteList &enetFrame) {enetFrame_ = enetFrame;}

	UInt32 send(Writable *channel);

private:
	PacketOut msg_;
	ActionList actions_;
	ByteList enetFrame_;

	friend struct llvm::yaml::MappingTraits<PacketOutBuilder>;
};

} // </namespace ofp>

#endif // OFP_PACKETOUT_H
