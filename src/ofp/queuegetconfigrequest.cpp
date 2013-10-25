//  ===== ---- ofp/queuegetconfigrequest.cpp ---------------*- C++ -*- =====  //
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
/// \brief Implements the QueueGetConfigRequest class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/queuegetconfigrequest.h"
#include "ofp/writable.h"

using namespace ofp;

bool QueueGetConfigRequest::validateLength(size_t length) const
{
	return length == sizeof(QueueGetConfigRequest);
}

QueueGetConfigRequestBuilder::QueueGetConfigRequestBuilder(const QueueGetConfigRequest *msg) : msg_{*msg}
{

}

UInt32 QueueGetConfigRequestBuilder::send(Writable *channel)
{
	UInt32 xid = channel->nextXid();
	UInt8 version = channel->version();
	size_t msgLen = sizeof(msg_);

	msg_.header_.setVersion(version);
	msg_.header_.setXid(xid);
	msg_.header_.setLength(UInt16_narrow_cast(msgLen));

	channel->write(&msg_, sizeof(msg_));
	channel->flush();

	return xid;
}