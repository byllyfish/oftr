//  ===== ---- ofp/getconfigreply.cpp ----------------------*- C++ -*- =====  //
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
/// \brief Implements GetConfigReply and GetConfigReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/getconfigreply.h"
#include "ofp/writable.h"

using namespace ofp;

const GetConfigReply *GetConfigReply::cast(const Message *message)
{
    assert(message->type() == OFPT_GET_CONFIG_REPLY);

    const GetConfigReply *msg = reinterpret_cast<const GetConfigReply *>(message->data());

    if (!msg->validateLength(message->size())) {
        return nullptr;
    }

    return msg;	
}

bool GetConfigReply::validateLength(size_t length) const
{
	return length == sizeof(GetConfigReply);
}

GetConfigReplyBuilder::GetConfigReplyBuilder(const GetConfigReply *msg)
    : msg_{*msg}
{
}

void GetConfigReplyBuilder::setFlags(UInt16 flags)
{
	msg_.flags_ = flags;
}

void GetConfigReplyBuilder::setMissSendLen(UInt16 missSendLen)
{
	msg_.missSendLen_ = missSendLen;
}

UInt32 GetConfigReplyBuilder::send(Writable *channel)
{
   UInt32 xid = channel->nextXid();

    msg_.header_.setVersion(channel->version());
    msg_.header_.setLength(sizeof(msg_));
    msg_.header_.setXid(xid);

    channel->write(&msg_, sizeof(msg_));
    channel->flush();

    return xid;
}

