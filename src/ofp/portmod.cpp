//  ===== ---- ofp/portmod.cpp -----------------------------*- C++ -*- =====  //
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
/// \brief Implements the PortMod class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/portmod.h"
#include "ofp/writable.h"

using namespace ofp;

bool PortMod::validateInput(size_t length) const {
  if (length != sizeof(PortMod))
    return false;

  return true;
}

PortModBuilder::PortModBuilder(const PortMod *msg) : msg_{*msg} {}

UInt32 PortModBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();
  size_t msgLen = sizeof(msg_);

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  if (version == OFP_VERSION_1) {
    // Temporarily set the message type to the V1 value.
    msg_.header_.setType(deprecated::v1::OFPT_PORT_MOD);
    msg_.header_.setLength(UInt16_narrow_cast(msgLen - 8));
    channel->write(&msg_.header_, sizeof(Header));
    msg_.header_.setType(OFPT_PORT_MOD);

    channel->write(BytePtr(&msg_.portNo_) + 2, 2);
    channel->write(&msg_.hwAddr_, 6);
    channel->write(&msg_.config_, 16);

  } else {
    msg_.header_.setLength(UInt16_narrow_cast(msgLen));
    channel->write(&msg_, sizeof(msg_));
  }
  channel->flush();

  return xid;
}
