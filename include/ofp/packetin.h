// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PACKETIN_H_
#define OFP_PACKETIN_H_

#include "ofp/protocolmsg.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/matchheader.h"
#include "ofp/buffernumber.h"
#include "ofp/tablenumber.h"

namespace ofp {

class PacketIn
    : public ProtocolMsg<PacketIn, OFPT_PACKET_IN, 18, 65535, false> {
 public:
  UInt8 version() const { return header_.version(); }

  BufferNumber bufferId() const { return bufferId_; }
  UInt16 totalLen() const;
  PortNumber inPort() const;
  UInt32 inPhyPort() const;
  UInt64 metadata() const;
  OFPPacketInReason reason() const;
  TableNumber tableID() const;
  UInt64 cookie() const;

  Match match() const;
  ByteRange enetFrame() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  BufferNumber bufferId_;
  Big16 totalLen_;
  Big<OFPPacketInReason> reason_;
  TableNumber tableID_;
  Big64 cookie_;

  MatchHeader matchHeader_;
  Padding<4> pad_;

  // Only PacketInBuilder can construct an instance.
  PacketIn() : header_{type()} {}

  const MatchHeader *matchHeader() const;

  template <class Type>
  Type offset(size_t offset) const {
    return *Interpret_cast<Type>(BytePtr(this) + offset);
  }

  bool validateInputV1(Validation *context) const;
  bool validateInputV2(Validation *context) const;
  bool validateInputV3(Validation *context) const;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 28,
    SizeWithoutMatchHeader = 24
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

  void setBufferId(BufferNumber bufferId) { msg_.bufferId_ = bufferId; }
  void setTotalLen(UInt16 totalLen) { msg_.totalLen_ = totalLen; }
  void setInPort(PortNumber inPort) { inPort_ = inPort; }
  void setInPhyPort(UInt32 inPhyPort) { inPhyPort_ = inPhyPort; }
  void setMetadata(UInt64 metadata) { metadata_ = metadata; }
  void setReason(OFPPacketInReason reason) { msg_.reason_ = reason; }
  void setTableID(TableNumber tableID) { msg_.tableID_ = tableID; }
  void setCookie(UInt64 cookie) { msg_.cookie_ = cookie; }

  void setEnetFrame(const ByteRange &enetFrame) { enetFrame_ = enetFrame; }

  UInt32 send(Writable *channel);

 private:
  PacketIn msg_;
  PortNumber inPort_ = 0;
  Big32 inPhyPort_ = 0;
  Big64 metadata_ = 0;
  MatchBuilder match_;
  ByteList enetFrame_;

  UInt32 sendV1(Writable *channel);
  UInt32 sendV2(Writable *channel);
  UInt32 sendV3(Writable *channel);
  UInt32 sendV4(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PACKETIN_H_
