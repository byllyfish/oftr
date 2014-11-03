//  ===== ---- ofp/groupmod.cpp ----------------------------*- C++ -*- =====  //
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
/// \brief Implements the GroupMod class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/groupmod.h"
#include "ofp/writable.h"

using namespace ofp;

BucketRange GroupMod::buckets() const {
  size_t len = header_.length();
  assert(len >= sizeof(GroupMod));

  return BucketRange{
      ByteRange{BytePtr(this) + sizeof(GroupMod), len - sizeof(GroupMod)}};
}

bool GroupMod::validateInput(Validation *context) const {
  if (!buckets().validateInput(context)) {
    return false;
  }
  return true;
}

GroupModBuilder::GroupModBuilder(const GroupMod *msg)
    : msg_{*msg}, buckets_{msg->buckets()} {}

UInt32 GroupModBuilder::send(Writable *channel) {
  UInt32 xid = channel->nextXid();
  size_t msgLen = sizeof(msg_) + buckets_.size();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(xid);

  channel->write(&msg_, sizeof(msg_));
  channel->write(buckets_.data(), buckets_.size());
  channel->flush();

  return xid;
}
