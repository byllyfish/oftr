//  ===== ---- ofp/flowremoved.cpp -------------------------*- C++ -*- =====  //
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
/// \brief Implements the FlowRemoved class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/flowremoved.h"
#include "ofp/message.h"
#include "ofp/writable.h"
#include "ofp/originalmatch.h"

using namespace ofp;

bool FlowRemoved::validateInput(Validation *context) const {
  size_t length = context->length();

  if (length != SizeWithoutMatchHeader + matchHeader_.paddedLength()) {
    log::debug("FlowRemoved: Invalid length.");
    return false;
  }

  if (!matchHeader_.validateInput(length - SizeWithoutMatchHeader)) {
    log::debug("FlowRemoved: Invalid match.");
    return false;
  }

  return true;
}

Match FlowRemoved::match() const { return Match{&matchHeader_}; }

FlowRemovedBuilder::FlowRemovedBuilder(const FlowRemoved *msg) : msg_{*msg} {
  // FIXME copy match...
}

UInt32 FlowRemovedBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  if (version == OFP_VERSION_1) {
    return sendOriginal(channel);
  } else if (version == OFP_VERSION_2) {
    return sendStandard(channel);
  }

  // Calculate length of FlowRemoved message up to end of match section. Then
  // pad it to a multiple of 8 bytes.
  size_t msgMatchLen = FlowRemoved::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  UInt32 xid = channel->nextXid();
  size_t msgLen = msgMatchLenPadded;

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(xid);

  // Fill in the match header.
  msg_.matchHeader_.setType(OFPMT_OXM);
  msg_.matchHeader_.setLength(sizeof(MatchHeader) + match_.size());

  channel->write(&msg_, FlowRemoved::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), msgMatchLenPadded - msgMatchLen);
  channel->flush();

  return xid;
}

UInt32 FlowRemovedBuilder::sendStandard(Writable *channel) {
  UInt8 version = channel->version();
  assert(version == OFP_VERSION_2);

  deprecated::StandardMatch stdMatch{match_.toRange()};

  size_t msgLen = FlowRemoved::SizeWithoutMatchHeader + sizeof(stdMatch);
  UInt32 xid = channel->nextXid();

  msg_.header_.setVersion(version);
  msg_.header_.setLength(UInt16_narrow_cast(msgLen));
  msg_.header_.setXid(xid);

  if (msg_.hardTimeout_ != 0) {
    log::info("FlowRemovedBuilder: hardTimeout not supported in version 2.");
    msg_.hardTimeout_ = 0;
  }

  channel->write(&msg_, FlowRemoved::SizeWithoutMatchHeader);
  channel->write(&stdMatch, sizeof(stdMatch));
  channel->flush();

  return xid;
}

UInt32 FlowRemovedBuilder::sendOriginal(Writable *channel) {
  UInt8 version = channel->version();
  assert(version <= OFP_VERSION_1);

  deprecated::OriginalMatch origMatch{match_.toRange()};

  constexpr UInt16 msgLen = sizeof(Header) + sizeof(origMatch) +
                            FlowRemoved::SizeWithoutMatchHeader -
                            sizeof(Header);
  static_assert(msgLen == 88, "Unexpected size.");

  UInt32 xid = channel->nextXid();

  msg_.header_.setVersion(version);
  msg_.header_.setLength(msgLen);
  msg_.header_.setXid(xid);

  if (msg_.tableId_ != 0) {
    log::info("FlowRemovedBuilder: tableId not supported in version 1.");
    msg_.tableId_ = 0;
  }

  if (msg_.hardTimeout_ != 0) {
    log::info("FlowRemovedBuilder: hardTimeout not supported in version 1.");
    msg_.hardTimeout_ = 0;
  }

  channel->write(&msg_, sizeof(Header));
  channel->write(&origMatch, sizeof(origMatch));
  channel->write(&msg_.cookie_,
                 FlowRemoved::SizeWithoutMatchHeader - sizeof(Header));
  channel->flush();

  return xid;
}
