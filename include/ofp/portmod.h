// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PORTMOD_H_
#define OFP_PORTMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/portnumber.h"

namespace ofp {

class PortMod : public ProtocolMsg<PortMod, OFPT_PORT_MOD, 40, 40> {
 public:
  PortNumber portNo() const { return portNo_; }
  EnetAddress hwAddr() const { return hwAddr_; }
  OFPPortConfigFlags config() const { return config_; }
  OFPPortConfigFlags mask() const { return mask_; }
  OFPPortFeaturesFlags advertise() const { return advertise_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  PortNumber portNo_;
  Padding<4> pad1_;
  EnetAddress hwAddr_;
  Padding<2> pad2_;
  Big<OFPPortConfigFlags> config_;
  Big<OFPPortConfigFlags> mask_;
  Big<OFPPortFeaturesFlags> advertise_;
  Padding<4> pad3_;

  // Only PortModBuilder can construct an actual instance.
  PortMod() : header_{type()} {}

  friend class PortModBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortMod) == 40, "Unexpected size.");
static_assert(IsStandardLayout<PortMod>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PortMod>(), "Expected trivially copyable.");

class PortModBuilder {
 public:
  PortModBuilder() = default;
  explicit PortModBuilder(const PortMod *msg);

  void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }
  void setHwAddr(const EnetAddress &hwAddr) { msg_.hwAddr_ = hwAddr; }
  void setConfig(OFPPortConfigFlags config) { msg_.config_ = config; }
  void setMask(OFPPortConfigFlags mask) { msg_.mask_ = mask; }
  void setAdvertise(OFPPortFeaturesFlags advertise) {
    msg_.advertise_ = advertise;
  }

  UInt32 send(Writable *channel);

 private:
  PortMod msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PORTMOD_H_
