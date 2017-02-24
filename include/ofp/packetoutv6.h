// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PACKETOUTV6_H_
#define OFP_PACKETOUTV6_H_

#include "ofp/actionlist.h"
#include "ofp/buffernumber.h"
#include "ofp/match.h"
#include "ofp/matchbuilder.h"
#include "ofp/padding.h"
#include "ofp/protocolmsg.h"

namespace ofp {

class PacketOutV6
    : public ProtocolMsg<PacketOutV6, OFPT_PACKET_OUT, 20, 65535, false> {
 public:
  BufferNumber bufferId() const { return bufferId_; }

  Match match() const { return Match{&matchHeader_}; }
  ActionRange actions() const;
  ByteRange enetFrame() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  BufferNumber bufferId_ = OFP_NO_BUFFER;
  Big16 actionsLen_ = 0;
  Padding<2> pad_;
  MatchHeader matchHeader_;
  Padding<4> pad2_;

  enum : size_t {
    UnpaddedSizeWithMatchHeader = 20,
    SizeWithoutMatchHeader = 16
  };

  // Only PacketOutV6Builder can construct an instance.
  PacketOutV6() : header_{type()} {}

  friend class PacketOutV6Builder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PacketOutV6) == 24, "Unexpected size.");
static_assert(IsStandardLayout<PacketOutV6>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PacketOutV6>(), "Expected trivially copyable.");

class PacketOutV6Builder {
 public:
  PacketOutV6Builder() = default;
  explicit PacketOutV6Builder(const PacketOutV6 *msg);

  MatchBuilder &match() { return match_; }

  void setBufferId(BufferNumber bufferId) { msg_.bufferId_ = bufferId; }
  void setMatch(const MatchBuilder &match) { match_ = match; }
  void setActions(const ActionRange &actions) { actions_ = actions; }
  void setEnetFrame(const ByteRange &enetFrame) { enetFrame_ = enetFrame; }

  UInt32 send(Writable *channel);

 private:
  PacketOutV6 msg_;
  MatchBuilder match_;
  ActionList actions_;
  ByteList enetFrame_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PACKETOUTV6_H_
