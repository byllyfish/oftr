//  ===== ---- ofp/echorequest.cpp -------------------------*- C++ -*- =====  //
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
/// \brief Implements EchoRequest and EchoRequestBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/echorequest.h"
#include "ofp/message.h"
#include "ofp/writable.h"

namespace ofp { // <namespace ofp>

ByteRange EchoRequest::echoData() const
{
    return ByteRange{BytePtr(this) + sizeof(Header),
                     header_.length() - sizeof(Header)};
}

bool EchoRequest::validateLength(size_t length) const
{
    return (length >= sizeof(Header));
}

UInt32 EchoRequestBuilder::send(Writable *channel)
{
    UInt8 version = channel->version();
    UInt32 xid = channel->nextXid();
    size_t msgLen = sizeof(Header) + data_.size();
    msg_.header_.setVersion(version);
    msg_.header_.setXid(xid);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));

    channel->write(&msg_, sizeof(msg_));
    channel->write(data_.data(), data_.size());
    channel->flush();

    return xid;
}

} // </namespace ofp>