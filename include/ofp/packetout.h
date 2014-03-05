//  ===== ---- ofp/packetout.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the PacketOut and PacketOutBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

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

  bool validateInput(size_t length) const;

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
