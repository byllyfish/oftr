//  ===== ---- ofp/setasync.h ------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the SetAsync and SetAsyncBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SETASYNC_H
#define OFP_SETASYNC_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

class SetAsync {
public:
	enum { Type = OFPT_SET_ASYNC };

	static const SetAsync *cast(const Message *message);

	SetAsync() : header_{Type} {}

	UInt32 masterPacketInMask() const;
	UInt32 slavePacketInMask() const;
	UInt32 masterPortStatusMask() const;
	UInt32 slavePortStatusMask() const;
	UInt32 masterFlowRemovedMask() const;
	UInt32 slaveFlowRemovedMask() const;

private:
	Header header_;
	Big32 packetInMask_[2];
	Big32 portStatusMask_[2];
	Big32 flowRemovedMask_[2];

	bool validateLength(size_t length) const;

	friend class SetAsyncBuilder;
};

static_assert(sizeof(SetAsync) == 32, "Unexpected size.");
static_assert(IsStandardLayout<SetAsync>(), "Expected standard layout.");

class SetAsyncBuilder {
public:
	SetAsyncBuilder() = default;

	void setMasterPacketInMask(UInt32 mask);
	void setSlavePacketInMask(UInt32 mask);
	void setMasterPortStatusMask(UInt32 mask);
	void setSlavePortStatusMask(UInt32 mask);
	void setMasterFlowRemovedMask(UInt32 mask);
	void setSlaveFlowRemovedMask(UInt32 mask);
	
	void send(Writable *channel);

private:
	SetAsync msg_;
};

} // </namespace ofp>

#endif // OFP_SETASYNC_H
