// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PORT_H_
#define OFP_PORT_H_

#include "ofp/byteorder.h"
#include "ofp/strings.h"
#include "ofp/padding.h"
#include "ofp/enetaddress.h"
#include "ofp/portnumber.h"

namespace ofp {

class Writable;
class Validation;

namespace deprecated {
class PortV1;
}  // namespace deprecated

// TODO(bfish): Make Port a NonCopyable.
class Port {
 public:
  // Port is a fixed size struct; no length field present.
  enum {
    ProtocolIteratorSizeOffset = PROTOCOL_ITERATOR_SIZE_FIXED,
    ProtocolIteratorAlignment = 8
  };

  PortNumber portNo() const { return portNo_; }
  const EnetAddress &hwAddr() const { return hwAddr_; }
  const PortNameStr &name() const { return name_; }
  OFPPortConfigFlags config() const { return config_; }
  OFPPortStateFlags state() const { return state_; }
  OFPPortFeaturesFlags curr() const { return curr_; }
  OFPPortFeaturesFlags advertised() const { return advertised_; }
  OFPPortFeaturesFlags supported() const { return supported_; }
  OFPPortFeaturesFlags peer() const { return peer_; }
  UInt32 currSpeed() const { return currSpeed_; }
  UInt32 maxSpeed() const { return maxSpeed_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  PortNumber portNo_;
  Padding<4> pad1_;
  EnetAddress hwAddr_;
  Padding<2> pad2_;
  PortNameStr name_;
  Big<OFPPortConfigFlags> config_;
  Big<OFPPortStateFlags> state_;
  Big<OFPPortFeaturesFlags> curr_;
  Big<OFPPortFeaturesFlags> advertised_;
  Big<OFPPortFeaturesFlags> supported_;
  Big<OFPPortFeaturesFlags> peer_;
  Big32 currSpeed_;
  Big32 maxSpeed_;

  // Only a PortBuilder can create an instance.
  Port() = default;

  friend class PortBuilder;
  friend class PortStatus;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(Port) == 64, "Unexpected size.");
static_assert(IsStandardLayout<Port>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Port>(), "Expected trivially copyable.");

class PortBuilder {
 public:
  PortBuilder() = default;
  explicit PortBuilder(const deprecated::PortV1 &port);

  void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }
  void setHwAddr(const EnetAddress &hwAddr) { msg_.hwAddr_ = hwAddr; }
  void setName(const PortNameStr &name) { msg_.name_ = name; }
  void setConfig(OFPPortConfigFlags config) { msg_.config_ = config; }
  void setState(OFPPortStateFlags state) { msg_.state_ = state; }
  void setCurr(OFPPortFeaturesFlags curr) { msg_.curr_ = curr; }
  void setAdvertised(OFPPortFeaturesFlags advertised) { msg_.advertised_ = advertised; }
  void setSupported(OFPPortFeaturesFlags supported) { msg_.supported_ = supported; }
  void setPeer(OFPPortFeaturesFlags peer) { msg_.peer_ = peer; }
  void setCurrSpeed(UInt32 currSpeed) { msg_.currSpeed_ = currSpeed; }
  void setMaxSpeed(UInt32 maxSpeed) { msg_.maxSpeed_ = maxSpeed; }

  const Port &toPort() const { return msg_; }

  void write(Writable *channel);
  void reset() {}

 private:
  Port msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

namespace deprecated {

class PortV1 {
 public:
  PortV1() = default;
  explicit PortV1(const Port &port);

  UInt16 portNo() const { return portNo_; }
  void setPortNo(UInt16 portNo) { portNo_ = portNo; }

  const EnetAddress &hwAddr() const { return hwAddr_; }
  void setHwAddr(const EnetAddress &hwAddr) { hwAddr_ = hwAddr; }

  const PortNameStr &name() const { return name_; }
  void setName(const PortNameStr &name) { name_ = name; }

  OFPPortConfigFlags config() const { return config_; }
  void setConfig(OFPPortConfigFlags config) { config_ = config; }

  OFPPortStateFlags state() const { return state_; }
  void setState(OFPPortStateFlags state) { state_ = state; }

  OFPPortFeaturesFlags curr() const { return OFPPortFeaturesFlagsConvertFromV1(curr_); }
  void setCurr(OFPPortFeaturesFlags curr) { curr_ = OFPPortFeaturesFlagsConvertToV1(curr); }

  OFPPortFeaturesFlags advertised() const { return OFPPortFeaturesFlagsConvertFromV1(advertised_); }
  void setAdvertised(OFPPortFeaturesFlags advertised) { advertised_ = OFPPortFeaturesFlagsConvertToV1(advertised); }

  OFPPortFeaturesFlags supported() const { return OFPPortFeaturesFlagsConvertFromV1(supported_); }
  void setSupported(OFPPortFeaturesFlags supported) { supported_ = OFPPortFeaturesFlagsConvertToV1(supported); }

  OFPPortFeaturesFlags peer() const { return OFPPortFeaturesFlagsConvertFromV1(peer_); }
  void setPeer(OFPPortFeaturesFlags peer) { peer_ = OFPPortFeaturesFlagsConvertToV1(peer); }

 private:
  Big16 portNo_;
  EnetAddress hwAddr_;
  PortNameStr name_;
  Big<OFPPortConfigFlags> config_;
  Big<OFPPortStateFlags> state_;
  Big<OFPPortFeaturesFlags> curr_;
  Big<OFPPortFeaturesFlags> advertised_;
  Big<OFPPortFeaturesFlags> supported_;
  Big<OFPPortFeaturesFlags> peer_;
};

static_assert(sizeof(PortV1) == 48, "Unexpected size.");
static_assert(IsStandardLayout<PortV1>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PortV1>(), "Expected trivially copyable.");

}  // namespace deprecated
}  // namespace ofp

#endif  // OFP_PORT_H_
