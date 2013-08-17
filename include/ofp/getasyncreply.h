#ifndef OFP_GETASYNCREPLY_H
#define OFP_GETASYNCREPLY_H

#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/getasyncreply.h"

namespace ofp { // <namespace ofp>

class GetAsyncReply {
public:
	enum { Type = OFPT_GET_ASYNC_REPLY };

	static const GetAsyncReply *cast(const Message *message);

	GetAsyncReply() : header_{Type} {}

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

	friend class GetAsyncReplyBuilder;
};

static_assert(sizeof(GetAsyncReply) == 32, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncReply>(), "Expected standard layout.");

class GetAsyncReplyBuilder {
public:
	explicit GetAsyncReplyBuilder(const GetAsyncRequest *request);

	void setMasterPacketInMask(UInt32 mask);
	void setSlavePacketInMask(UInt32 mask);
	void setMasterPortStatusMask(UInt32 mask);
	void setSlavePortStatusMask(UInt32 mask);
	void setMasterFlowRemovedMask(UInt32 mask);
	void setSlaveFlowRemovedMask(UInt32 mask);

	UInt32 send(Writable *channel);

private:
	GetAsyncReply msg_;
};

} // </namespace ofp>

#endif // OFP_GETASYNCREPLY_H
