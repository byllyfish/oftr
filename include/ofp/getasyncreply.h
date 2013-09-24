//  ===== ---- ofp/getasyncreply.h -------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the GetAsyncReply and GetAsyncReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_GETASYNCREPLY_H
#define OFP_GETASYNCREPLY_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

class GetAsyncReply {
public:
	static constexpr OFPType type() { return OFPT_GET_ASYNC_REPLY; }
	static const GetAsyncReply *cast(const Message *message) { return message->cast<GetAsyncReply>(); }

	GetAsyncReply() : header_{type()} {}

	UInt32 masterPacketInMask() const;
	UInt32 slavePacketInMask() const;
	UInt32 masterPortStatusMask() const;
	UInt32 slavePortStatusMask() const;
	UInt32 masterFlowRemovedMask() const;
	UInt32 slaveFlowRemovedMask() const;

	bool validateLength(size_t length) const { return length == sizeof(GetAsyncReply); }

private:
	Header header_;
	Big32 packetInMask_[2];
	Big32 portStatusMask_[2];
	Big32 flowRemovedMask_[2];

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
