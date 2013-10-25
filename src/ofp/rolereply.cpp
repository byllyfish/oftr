//  ===== ---- ofp/rolereply.cpp ---------------------------*- C++ -*- =====  //
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
/// \brief Implements RoleReply and RoleReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/rolereply.h"
#include "ofp/writable.h"

using namespace ofp;

bool RoleReply::validateLength(size_t length) const {
  return length == sizeof(RoleReply);
}

RoleReplyBuilder::RoleReplyBuilder(UInt32 xid) {
  // Set xid of reply to request's xid.
  msg_.header_.setXid(xid);
}

RoleReplyBuilder::RoleReplyBuilder(const RoleRequest *request) {
  msg_.header_.setXid(request->xid());
}

RoleReplyBuilder::RoleReplyBuilder(const RoleReply *msg) : msg_{*msg} {}

UInt32 RoleReplyBuilder::send(Writable *channel) {
  size_t msgLen = sizeof(msg_);
  UInt8 version = channel->version();

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  channel->write(&msg_, sizeof(msg_));
  channel->flush();

  return msg_.header_.xid();
}