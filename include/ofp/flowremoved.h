//  ===== ---- ofp/flowremoved.h ---------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the FlowRemoved and FlowRemovedBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_FLOWREMOVED_H
#define OFP_FLOWREMOVED_H

namespace ofp { // <namespace ofp>

class FlowRemoved {
public:
	static constexpr OFPType type() { return OFPT_FLOW_REMOVED; }
	static const FlowRemoved *cast(const Message *message);

	FlowRemoved() : header_{type()} {}

	/// Opaque controller-issued identifier.
	UInt64 cookie() const { return cookie_; }

	/// Priority level of flow entry.
	UInt16 priority() const { return priority_; }

	/// One of OFPRR_*.
	UInt8 reason() const { return reason_; }

	/// ID of the table
	UInt8 tableID() const { return tableID_; }

	/// Time flow was alive in seconds.
	UInt32 durationSec() const { return durationSec_; }

	/// Time flow was alive in nanoseconds beyond durationSec.
	UInt32 durationNSec() const { return durationNsec_; }

	/// Idle timeout from original flow mod.
	UInt16 idleTimeout() const { return idleTimeout_; }

	/// Hard timeout from original flow mod.
	UInt16 hardTimeout() const { return hardTimeout_; }

	/// Number of packets that were associated with the entry.
	UInt64 packetCount() const { return packetCount_; }

	/// Number of the bytes that were associated with the entry.
	UInt64 byteCount() const { return byteCount_; }

	bool validateLength(size_t length) const;

private:
	Header header_;
	Big64 cookie_;
	Big16 priority_;
	Big8 reason_;
	Big8 tableID_;
	Big32 durationSec_;
	Big32 durationNsec_;
	Big16 idleTimeout_;
	Big16 hardTimeout_;
	Big64 packetCount_;
	Big64 byteCount_;

	Big16 matchType_ = 0;
    Big16 matchLength_ = 0;
    Padding<4> pad_2;

	friend class FlowRemovedBuilder;
};

static_assert(sizeof(FlowRemoved) == 56, "Unexpected size.");
static_assert(IsStandardLayout<FlowRemoved>(), "Expected standard layout.");

class FlowRemovedBuilder {
public:
	FlowRemovedBuilder() = default;

	// TODO setters

	UInt32 send(Writable *channel);

private:
	FlowRemoved msg_;
};

} // </namespace ofp>

#endif // OFP_FLOWREMOVED_H
