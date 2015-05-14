// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PORTMOD_H_
#define OFP_PORTMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/portnumber.h"
#include "ofp/propertylist.h"

namespace ofp {

class PortMod : public ProtocolMsg<PortMod, OFPT_PORT_MOD, 32> {
 public:
  PortNumber portNo() const { return portNo_; }
  EnetAddress hwAddr() const { return hwAddr_; }
  OFPPortConfigFlags config() const { return config_; }
  OFPPortConfigFlags mask() const { return mask_; }

  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  PortNumber portNo_;
  Padding<4> pad1_;
  EnetAddress hwAddr_;
  Padding<2> pad2_;
  Big<OFPPortConfigFlags> config_;
  Big<OFPPortConfigFlags> mask_;

  // Only PortModBuilder can construct an actual instance.
  PortMod() : header_{type()} {}

  friend class PortModBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortMod) == 32, "Unexpected size.");
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

  void setProperties(const PropertyList &properties) { properties_ = properties; }

  UInt32 send(Writable *channel);

 private:
  PortMod msg_;
  PropertyList properties_;

  void sendV1(Writable *channel);

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PORTMOD_H_
