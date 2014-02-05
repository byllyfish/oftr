//  ===== ---- ofp/packetin.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the PacketIn and PacketInBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PACKETIN_H_
#define OFP_PACKETIN_H_

#include "ofp/protocolmsg.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"

namespace ofp {

class PacketIn : public ProtocolMsg<PacketIn, OFPT_PACKET_IN, 20, 65535, false> {
public:
  UInt8 version() const { return header_.version(); }

  UInt32 bufferId() const { return bufferId_; }

  UInt16 totalLen() const;
  UInt32 inPort() const;
  UInt32 inPhyPort() const;
  UInt64 metadata() const;
  OFPPacketInReason reason() const;
  UInt8 tableID() const;
  UInt64 cookie() const;

  ByteRange enetFrame() const;

  bool validateLength(size_t length) const;

private:
  Header header_;
  Big32 bufferId_;
  Big16 totalLen_;
  OFPPacketInReason reason_;
  Big8 tableID_;
  Big64 cookie_;

  Big16 matchType_ = 0;
  Big16 matchLength_ = 0;
  Padding<4> pad_;

  // Only PacketInBuilder can construct an instance.
  PacketIn() : header_{type()} {}

  OXMRange oxmRange() const;

  template <class Type>
  Type offset(size_t offset) const {
    return *reinterpret_cast<const Type *>(BytePtr(this) + offset);
  }

  bool validateLengthV1(size_t length) const;
  bool validateLengthV2(size_t length) const;
  bool validateLengthV3(size_t length) const;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 28,
    SizeWithoutMatchHeader = 24,
    MatchHeaderSize = 4,
  };

  friend class PacketInBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PacketIn) == 32, "Unexpected size.");
static_assert(IsStandardLayout<PacketIn>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PacketIn>(), "Expected trivially copyable.");

class PacketInBuilder {
public:
  PacketInBuilder() = default;
  explicit PacketInBuilder(const PacketIn *msg);

  void setBufferId(UInt32 bufferId) { msg_.bufferId_ = bufferId; }
  void setTotalLen(UInt16 totalLen) { msg_.totalLen_ = totalLen; }
  void setInPort(UInt32 inPort) { inPort_ = inPort; }
  void setInPhyPort(UInt32 inPhyPort) { inPhyPort_ = inPhyPort; }
  void setMetadata(UInt64 metadata) { metadata_ = metadata; }
  void setReason(OFPPacketInReason reason) { msg_.reason_ = reason; }
  void setTableID(UInt8 tableID) { msg_.tableID_ = tableID; }
  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }

  void setEnetFrame(const ByteRange &enetFrame) { enetFrame_ = enetFrame; }

  UInt32 send(Writable *channel);

private:
  PacketIn msg_;
  Big32 inPort_ = 0;
  Big32 inPhyPort_ = 0;
  Big64 metadata_ = 0;
  MatchBuilder match_;
  ByteList enetFrame_;

  UInt32 sendV1(Writable *channel);
  UInt32 sendV2(Writable *channel);
  UInt32 sendV3(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PACKETIN_H_
