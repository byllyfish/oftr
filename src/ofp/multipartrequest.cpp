//  ===== ---- ofp/multipartrequest.cpp --------------------*- C++ -*- =====  //
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
/// \brief Implements MultipartRequest class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/multipartrequest.h"
#include "ofp/writable.h"
#include "ofp/message.h"
#include "ofp/mptablefeatures.h"

using namespace ofp;

template <class Type>
static bool validateBody(const UInt8 *body, size_t length) {
  const Type *ptr = reinterpret_cast<const Type *>(body);
  return ptr->validateInput(length);
}

// TODO(bfish): wrap this in an MPRange<Type> or something...
template <class Type>
static bool validateBodyArray(const UInt8 *body, size_t length) {
  while (length != 0) {
    const Type *ptr = reinterpret_cast<const Type *>(body);
    assert(IsPtrAligned(ptr, 8) && "Misaligned ptr");
    size_t size = ptr->length();

    if (size < 8 || size > length) return false;
    if ((size % 8) != 0) return false;
    if (!ptr->validateInput(length)) return false;
    body += size;
    length -= size;
  }
  return true;
}



bool MultipartRequest::validateInput(size_t length) const {
  // FIXME - this will need to cast and verify the lengths in the body.

  assert(length == requestBodySize() + 16);

  switch (requestType()) {
    case OFPMP_TABLE_FEATURES:
      return validateBodyArray<MPTableFeatures>(requestBody(), requestBodySize());
  }

  return true;
}

UInt32 MultipartRequestBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();
  
  size_t msgLen = body_.size();
  if (version == OFP_VERSION_1) {
    msgLen += MultipartRequest::UnpaddedSizeVersion1;
  } else {
    msgLen += sizeof(msg_);
  }

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));

  if (version == OFP_VERSION_1) {
    msg_.header_.setType(deprecated::v1::OFPT_STATS_REQUEST);
    channel->write(&msg_, MultipartRequest::UnpaddedSizeVersion1);
  } else {
    msg_.header_.setType(OFPT_MULTIPART_REQUEST);
    channel->write(&msg_, sizeof(msg_));
  }
  
  channel->write(body_.data(), body_.size());
  channel->flush();

  return xid;
}

