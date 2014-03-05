//  ===== ---- ofp/portstatus.cpp --------------------------*- C++ -*- =====  //
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
/// \brief Implements PortStatus and PortStatusBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/portstatus.h"
#include "ofp/message.h"
#include "ofp/writable.h"

using namespace ofp;

bool PortStatus::validateInput(size_t length) const {
  if (length != sizeof(PortStatus)) {
    log::info("Invalid size of PortStatus message:", length);
    return false;
  }

  return true;
}

PortStatusBuilder::PortStatusBuilder(const PortStatus *msg) : msg_{*msg} {}

UInt32 PortStatusBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();

  UInt16 msgLen = sizeof(msg_);
  if (version == OFP_VERSION_1) {
    // Subtract difference between size of Port and PortV1.
    msgLen -= 16;
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  if (version == OFP_VERSION_1) {
    deprecated::PortV1 port{msg_.port_};
    channel->write(&msg_, offsetof(PortStatus, port_));
    channel->write(&port, sizeof(port));
  } else {
    channel->write(&msg_, sizeof(msg_));
  }
  channel->flush();

  return xid;
}
