// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PACKETOUT_H_
#define OFP_PACKETOUT_H_

#include "ofp/protocolmsg.h"
#include "ofp/actionlist.h"
#include "ofp/padding.h"

namespace ofp {

class PacketOut
    : public ProtocolMsg<PacketOut, OFPT_PACKET_OUT, 24, 65535, false> {
 public:
  UInt32 bufferId() const { return bufferId_; }
  UInt32 inPort() const { return inPort_; }

  ActionRange actions() const;
  ByteRange enetFrame() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big32 bufferId_ = OFP_NO_BUFFER;
  Big32 inPort_ = 0;
  Big16 actionsLen_ = 0;
  Padding<6> pad_;

  // Only PacketOutBuilder can construct an instance.
  PacketOut() : header_{type()} {}

  friend class PacketOutBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PacketOut) == 24, "Unexpected size.");
static_assert(IsStandardLayout<PacketOut>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PacketOut>(), "Expected trivially copyable.");

class PacketOutBuilder {
 public:
  PacketOutBuilder() = default;
  explicit PacketOutBuilder(const PacketOut *msg);

  void setBufferId(UInt32 bufferId) { msg_.bufferId_ = bufferId; }
  void setInPort(UInt32 inPort) { msg_.inPort_ = inPort; }
  void setActions(const ActionRange &actions) { actions_ = actions; }
  void setEnetFrame(const ByteRange &enetFrame) { enetFrame_ = enetFrame; }

  UInt32 send(Writable *channel);

 private:
  PacketOut msg_;
  ActionList actions_;
  ByteList enetFrame_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PACKETOUT_H_
