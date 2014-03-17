//  ===== ---- ofp/flowmod.cpp -----------------------------*- C++ -*- =====  //
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
/// \brief Implements FlowMod and FlowModBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/flowmod.h"
#include "ofp/writable.h"
#include "ofp/originalmatch.h"

namespace ofp { // <namespace ofp>

Match FlowMod::match() const {
  assert(validateInput(header_.length()));

  UInt16 type = matchType_;

  if (type == OFPMT_OXM) {
    assert(matchLength_ >= MatchHeaderSize);
    OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader,
                   matchLength_ - MatchHeaderSize};
    return Match{range};

  } else if (type == OFPMT_STANDARD) {
    assert(matchLength_ == deprecated::OFPMT_STANDARD_LENGTH);
    const deprecated::StandardMatch *stdMatch =
        reinterpret_cast<const deprecated::StandardMatch *>(
            BytePtr(this) + SizeWithoutMatchHeader);
    return Match{stdMatch};

  } else {
    log::debug("Unknown matchType:", type);
    return Match{OXMRange{nullptr, 0}};
  }
}

InstructionRange FlowMod::instructions() const {
  size_t offset = PadLength(SizeWithoutMatchHeader + matchLength_);
  assert(header_.length() >= offset);

  return InstructionRange{
      ByteRange{BytePtr(this) + offset, header_.length() - offset}};
}

bool FlowMod::validateInput(size_t length) const {
  if (length < UnpaddedSizeWithMatchHeader) {
    log::debug("FlowMod too small", length);
    return false;
  }

  // Check the match length.
  UInt16 matchLen = matchLength_;
  if (matchLen < MatchHeaderSize) {
    log::debug("Match header size too small", matchLen);
    return false;
  }

  if (length < SizeWithoutMatchHeader + matchLen) {
    log::debug("FlowMod too small with match", length);
    return false;
  }

  UInt16 matchType = matchType_;
  if (matchType == OFPMT_OXM) {
    OXMRange range{BytePtr(this) + UnpaddedSizeWithMatchHeader,
                   matchLen - MatchHeaderSize};
    if (!range.validateInput()) {
      log::debug("FlowMod OXM list has length mismatch");
      return false;
    }

  } else if (matchType == OFPMT_STANDARD) {
    if (matchLen != deprecated::OFPMT_STANDARD_LENGTH) {
      log::debug("FlowMod StandardMatch unexpected length", matchLen);
      return false;
    }
  }

  return true;
}

FlowModBuilder::FlowModBuilder(const FlowMod *msg) : msg_{*msg} {
  setMatch(msg->match());
  setInstructions(msg->instructions());
}

UInt32 FlowModBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  if (version <= OFP_VERSION_1) {
    return sendOriginal(channel);
  } else if (version == OFP_VERSION_2) {
    return sendStandard(channel);
  }

  // Calculate length of FlowMod message up to end of match section. Then
  // pad it to a multiple of 8 bytes.
  size_t msgMatchLen = FlowMod::UnpaddedSizeWithMatchHeader + match_.size();
  size_t msgMatchLenPadded = PadLength(msgMatchLen);

  // Calculate length of instruction section. Then, pad it to a multiple
  // of 8 bytes.
  size_t instrLen = instructions_.size();
  size_t instrLenPadded = PadLength(instrLen);
  assert((instrLen % 8) == 0);        // should already be padded, right?

  // Calculate the total FlowMod message length.
  size_t msgLen = msgMatchLenPadded + instrLenPadded;

  // Fill in the message header.
  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(version);
  hdr.setType(FlowMod::type());
  hdr.setLength(UInt16_narrow_cast(msgLen));
  hdr.setXid(xid);

  // Fill in the match header.
  msg_.matchType_ = OFPMT_OXM;
  msg_.matchLength_ =
      UInt16_narrow_cast(FlowMod::MatchHeaderSize + match_.size());

  // Write the message with padding in the correct spots.
  channel->write(&msg_, FlowMod::UnpaddedSizeWithMatchHeader);
  channel->write(match_.data(), match_.size(), msgMatchLenPadded - msgMatchLen);
  channel->write(instructions_.data(), instructions_.size());
  channel->flush();

  return xid;
}

UInt32 FlowModBuilder::sendStandard(Writable *channel) {
  UInt8 version = channel->version();
  assert(version >= OFP_VERSION_2);

  deprecated::StandardMatch stdMatch{match_.toRange()};

  size_t msgMatchLen = FlowMod::SizeWithoutMatchHeader + sizeof(stdMatch);
  size_t instrLen = instructions_.size();
  size_t instrLenPadded = PadLength(instrLen);
  size_t msgLen = msgMatchLen + instrLenPadded;

  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(version);
  hdr.setType(FlowMod::type());
  hdr.setLength(UInt16_narrow_cast(msgLen));
  hdr.setXid(xid);

  channel->write(&msg_, FlowMod::SizeWithoutMatchHeader);
  channel->write(&stdMatch, sizeof(stdMatch));
  channel->write(instructions_.data(), instrLen);
  channel->flush();

  return xid;
}

UInt32 FlowModBuilder::sendOriginal(Writable *channel) {
  UInt8 version = channel->version();
  assert(version <= OFP_VERSION_1);

  deprecated::OriginalMatch origMatch{match_.toRange()};
  ActionRange actions = instructions_.toRange().outputActions();

  UInt16 msgLen = 72;
  if (actions.size() > 0) {
    msgLen += actions.writeSize(channel);
  }

  UInt32 xid = channel->nextXid();
  Header &hdr = msg_.header_;
  hdr.setVersion(version);
  hdr.setType(FlowMod::type());
  hdr.setLength(msgLen);
  hdr.setXid(xid);

  if (msg_.tableId_ != 0) {
    log::info("FlowModBuilder: tableId not supported in version 1.");
    msg_.tableId_ = 0;
  }

  channel->write(&msg_, sizeof(Header));
  channel->write(&origMatch, sizeof(origMatch));
  channel->write(&msg_.cookie_, 8);
  channel->write(&msg_.tableId_, 12);
  channel->write(BytePtr(&msg_.outPort_) + 2, 2);   // Big-Endian -> Big-Endian
  channel->write(&msg_.flags_, 2);

  if (actions.size() > 0) {
    actions.write(channel);
  }

  channel->flush();

  return xid;
}

// The fastest it can be done... useful for self-benchmarking.
UInt32 FlowModBuilder::sendFastVersion1(Writable *channel, UInt32 inPort, UInt32 outPort, UInt32 bufferId,
                    const EnetAddress &dst, const EnetAddress &src)
{
  const char * const layout = 
    "\x01\x0E\x00\x50\x00\x00\x00\x01"
    "\x00\x3F\xFF\xF2\xA2\xA2\xCD\xCD"
    "\xCD\xCD\xCD\xCD\xAB\xAB\xAB\xAB"
    "\xAB\xAB\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x0A\x00\x1E\x80\x00"
    "\x34\x34\x34\x34\x00\x00\x00\x00"
    "\x00\x00\x00\x08\xB3\xB3\x00\x00";

  OFP_ALIGNAS(8) UInt8 buf[80];
  assert(IsPtrAligned<8>(buf));

  UInt32 xid = channel->nextXid();

  std::memcpy(buf, layout, sizeof(buf));
  *Big32_cast(buf + 4) = xid;
  *Big16_cast(buf + 12) = UInt16_narrow_cast(inPort);
  std::memcpy(buf + 14, &src, 6);
  std::memcpy(buf + 20, &dst, 6);
  *Big32_cast(buf + 64) = bufferId;
  *Big16_cast(buf + 76) = UInt16_narrow_cast(outPort);

  channel->write(buf, sizeof(buf));
  channel->flush();
  
  return xid;
}

} // </namespace ofp>
