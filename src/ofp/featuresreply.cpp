//  ===== ---- ofp/featuresreply.cpp -----------------------*- C++ -*- =====  //
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
/// \brief Implements FeaturesReply and FeaturesReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/featuresreply.h"
#include "ofp/channel.h"
#include "ofp/log.h"

using namespace ofp;

bool FeaturesReply::validateInput(Validation *context) const {
  UInt8 version = header_.version();

  // Version 4 and later don't include the port list.
  if (version >= OFP_VERSION_4) {
    return (context->length() == sizeof(FeaturesReply));
  }

  // Make sure the size of the list is a multiple of the port size. (Note
  // that we mean post-1.0 port size; validation happens after transmogrify.)
  size_t portListSize = header_.length() - sizeof(FeaturesReply);

  return (portListSize % sizeof(Port)) == 0;
}

PortRange FeaturesReply::ports() const {
  assert(header_.length() >= sizeof(FeaturesReply));

  return ByteRange{BytePtr(this) + sizeof(FeaturesReply),
                   header_.length() - sizeof(FeaturesReply)};
}

FeaturesReplyBuilder::FeaturesReplyBuilder(const FeaturesReply *msg)
    : msg_{*msg} {}

FeaturesReplyBuilder::FeaturesReplyBuilder(UInt32 xid) {
  // Set xid of reply to request's xid.
  msg_.header_.setXid(xid);
}

UInt32 FeaturesReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  PortRange portRange{ports_};

  size_t msgLen = sizeof(msg_);
  if (version < OFP_VERSION_4) {
    msgLen += portRange.writeSize(channel);
  }

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  channel->write(&msg_, sizeof(msg_));

  if (version < OFP_VERSION_4) {
    portRange.write(channel);
  }

  channel->flush();

  return msg_.header_.xid();
}
