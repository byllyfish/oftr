//  ===== ---- ofp/error.cpp -------------------------------*- C++ -*- =====  //
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
/// \brief Implements Error and ErrorBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/error.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/log.h"

using namespace ofp;

ByteRange Error::errorData() const {
  return ByteRange{BytePtr(this) + sizeof(Error),
                   header_.length() - sizeof(Error)};
}

ErrorBuilder::ErrorBuilder(UInt32 xid) {
  msg_.header_.setXid(xid);
}

ErrorBuilder::ErrorBuilder(const Error *msg) : msg_{*msg} {
  // Only data is left to copy.
  ByteRange data = msg->errorData();
  setErrorData(data.data(), data.size());
}

void ErrorBuilder::setErrorData(const Message *message) {
  assert(message);
  size_t len = message->size();
  if (len > 64)
    len = 64;
  setErrorData(message->data(), len);
}

void ErrorBuilder::send(Writable *channel) {
  size_t msgLen = sizeof(msg_) + data_.size();

  msg_.header_.setVersion(channel->version());
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(channel->nextXid());

  channel->write(&msg_, sizeof(msg_));
  channel->write(data_.data(), data_.size());
  channel->flush();
}
