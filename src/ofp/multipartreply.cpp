//  ===== ---- ofp/multipartreply.cpp ----------------------*- C++ -*- =====  //
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
/// \brief Implements MultipartReply class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/multipartreply.h"
#include "ofp/writable.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

bool MultipartReply::validateInput(Validation *context) const {
  // FIXME - see same method in MultipartRequest.
  return true;
}

UInt32 MultipartReplyBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();

  size_t msgLen = body_.size();
  if (version == OFP_VERSION_1) {
    msgLen += MultipartReply::UnpaddedSizeVersion1;
  } else {
    msgLen += sizeof(msg_);
  }

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  if (version == OFP_VERSION_1) {
    msg_.header_.setType(deprecated::v1::OFPT_STATS_REPLY);
    channel->write(&msg_, MultipartReply::UnpaddedSizeVersion1);
  } else {
    msg_.header_.setType(OFPT_MULTIPART_REPLY);
    channel->write(&msg_, sizeof(msg_));
  }

  channel->write(body_.data(), body_.size());
  channel->flush();

  return xid;
}

} // </namespace ofp>
