//  ===== ---- ofp/packetout.cpp ---------------------------*- C++ -*- =====  //
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
/// \brief Implements PacketOut and PacketOutBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/packetout.h"
#include "ofp/message.h"

using namespace ofp;

bool PacketOut::validateLength(size_t length) const {
  if (length < sizeof(PacketOut) + actionsLen_) {
    log::info("PacketOut::validateLength: Too short.", length);
    return false;
  }

  // FIXME validate actions

  return true;
}

ActionRange PacketOut::actions() const {
  return ActionRange{ByteRange{BytePtr(this) + sizeof(PacketOut), actionsLen_}};
}

ByteRange PacketOut::enetFrame() const {
  size_t offset = sizeof(PacketOut) + actionsLen_;
  return ByteRange{BytePtr(this) + offset, header_.length() - offset};
}

PacketOutBuilder::PacketOutBuilder(const PacketOut *msg) : msg_{*msg} {
  setActions(msg->actions());
  setEnetFrame(msg->enetFrame());
}

UInt32 PacketOutBuilder::send(Writable *channel) {
  UInt8 version = channel->version();

  size_t actionSize = actions_.toRange().writeSize(channel);

  size_t msgLen = sizeof(msg_) + actionSize;
  if (version == OFP_VERSION_1) {
    msgLen -= 8;
  }

  if (msg_.bufferId() != OFP_NO_BUFFER) {
    msgLen += enetFrame_.size();
  }

  // Fill in the message header.
  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(version);
  hdr.setType(PacketOut::type());
  hdr.setLength(UInt16_narrow_cast(msgLen));
  hdr.setXid(xid);

  // Fill in length of actions section.
  msg_.actionsLen_ = UInt16_narrow_cast(actionSize);

  // Write main part of message.
  if (version == OFP_VERSION_1) {
    // Up to in_port
    channel->write(&msg_, 12);
    // 16-bit in_port
    channel->write(BytePtr(&msg_.inPort_) + 2, 2);
    channel->write(&msg_.actionsLen_, 2);
  } else {
    channel->write(&msg_, sizeof(msg_));
  }

  // Write actions to channel.
  actions_.toRange().write(channel);

  if (msg_.bufferId() != OFP_NO_BUFFER) {
    channel->write(enetFrame_.data(), enetFrame_.size());
  }
  channel->flush();

  return xid;
}
