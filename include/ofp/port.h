// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PORT_H_
#define OFP_PORT_H_

#include "ofp/byteorder.h"
#include "ofp/strings.h"
#include "ofp/padding.h"
#include "ofp/macaddress.h"
#include "ofp/portnumber.h"
#include "ofp/propertylist.h"
#include "ofp/portproperty.h"

namespace ofp {

class Writable;
class Validation;

namespace deprecated {
class PortV1;
class PortV2;
}  // namespace deprecated

class Port : private NonCopyable {
 public:
  enum { ProtocolIteratorSizeOffset = 4, ProtocolIteratorAlignment = 8 };

  enum { MPVariableSizeOffset = 4 };

  static const size_t DefaultSizeEthernet = 40 + sizeof(PortPropertyEthernet);

  PortNumber portNo() const { return portNo_; }
  const MacAddress &hwAddr() const { return hwAddr_; }
  const PortNameStr &name() const { return name_; }
  OFPPortConfigFlags config() const { return config_; }
  OFPPortStateFlags state() const { return state_; }

  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  PortNumber portNo_;
  Big16 length_ = 40;
  Padding<2> pad1_;
  MacAddress hwAddr_;
  Padding<2> pad2_;
  PortNameStr name_;
  Big<OFPPortConfigFlags> config_;
  Big<OFPPortStateFlags> state_;

  // Only a PortBuilder can create an instance.
  Port() = default;
  Port(const Port &) = default;

  friend class PortList;
  friend class PortBuilder;
  friend class PortStatus;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(Port) == 40, "Unexpected size.");
static_assert(IsStandardLayout<Port>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Port>(), "Expected trivially copyable.");

class PortBuilder {
 public:
  PortBuilder() = default;
  explicit PortBuilder(const deprecated::PortV1 &port);
  explicit PortBuilder(const deprecated::PortV2 &port);

  void setPortNo(PortNumber portNo) { msg_.portNo_ = portNo; }
  void setHwAddr(const MacAddress &hwAddr) { msg_.hwAddr_ = hwAddr; }
  void setName(const PortNameStr &name) { msg_.name_ = name; }
  void setConfig(OFPPortConfigFlags config) { msg_.config_ = config; }
  void setState(OFPPortStateFlags state) { msg_.state_ = state; }

  void setProperties(PropertyRange properties) {
    properties_.assign(properties);
    updateLen();
  }
  void setProperties(const PropertyList &properties) {
    properties_ = properties;
    updateLen();
  }

  size_t writeSize(Writable *channel);
  void write(Writable *channel);
  void reset() {}

  void copyTo(UInt8 *ptr);

  // Custom assignment operator because msg_ is non-copyable.
  PortBuilder &operator=(const PortBuilder &port);

 private:
  Port msg_;
  PropertyList properties_;

  enum { SizeWithoutProperties = sizeof(msg_) };

  void updateLen() {
    msg_.length_ =
        UInt16_narrow_cast(SizeWithoutProperties + properties_.size());
  }

  friend class PortList;
  friend class deprecated::PortV1;
  friend class deprecated::PortV2;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

namespace deprecated {

/// Port structure from OpenFlow 1.0.
class PortV1 {
 public:
  PortV1() = default;
  explicit PortV1(const Port &port);
  explicit PortV1(const PortBuilder &portBuilder);

  UInt16 portNo() const { return portNo_; }
  void setPortNo(UInt16 portNo) { portNo_ = portNo; }

  const MacAddress &hwAddr() const { return hwAddr_; }
  void setHwAddr(const MacAddress &hwAddr) { hwAddr_ = hwAddr; }

  const PortNameStr &name() const { return name_; }
  void setName(const PortNameStr &name) { name_ = name; }

  OFPPortConfigFlags config() const { return config_; }
  void setConfig(OFPPortConfigFlags config) { config_ = config; }

  OFPPortStateFlags state() const { return state_; }
  void setState(OFPPortStateFlags state) { state_ = state; }

  OFPPortFeaturesFlags curr() const {
    return OFPPortFeaturesFlagsConvertFromV1(curr_);
  }
  void setCurr(OFPPortFeaturesFlags curr) {
    curr_ = OFPPortFeaturesFlagsConvertToV1(curr);
  }

  OFPPortFeaturesFlags advertised() const {
    return OFPPortFeaturesFlagsConvertFromV1(advertised_);
  }
  void setAdvertised(OFPPortFeaturesFlags advertised) {
    advertised_ = OFPPortFeaturesFlagsConvertToV1(advertised);
  }

  OFPPortFeaturesFlags supported() const {
    return OFPPortFeaturesFlagsConvertFromV1(supported_);
  }
  void setSupported(OFPPortFeaturesFlags supported) {
    supported_ = OFPPortFeaturesFlagsConvertToV1(supported);
  }

  OFPPortFeaturesFlags peer() const {
    return OFPPortFeaturesFlagsConvertFromV1(peer_);
  }
  void setPeer(OFPPortFeaturesFlags peer) {
    peer_ = OFPPortFeaturesFlagsConvertToV1(peer);
  }

 private:
  Big16 portNo_;
  MacAddress hwAddr_;
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

/// Port structure from OpenFlow 1.1 - 1.3.
class PortV2 {
 public:
  PortV2() = default;
  explicit PortV2(const Port &port);
  explicit PortV2(const PortBuilder &portBuilder);

  PortNumber portNo() const { return portNo_; }
  void setPortNo(PortNumber portNo) { portNo_ = portNo; }

  const MacAddress &hwAddr() const { return hwAddr_; }
  void setHwAddr(const MacAddress &hwAddr) { hwAddr_ = hwAddr; }

  const PortNameStr &name() const { return name_; }
  void setName(const PortNameStr &name) { name_ = name; }

  OFPPortConfigFlags config() const { return config_; }
  void setConfig(OFPPortConfigFlags config) { config_ = config; }

  OFPPortStateFlags state() const { return state_; }
  void setState(OFPPortStateFlags state) { state_ = state; }

  OFPPortFeaturesFlags curr() const { return curr_; }
  void setCurr(OFPPortFeaturesFlags curr) { curr_ = curr; }

  OFPPortFeaturesFlags advertised() const { return advertised_; }
  void setAdvertised(OFPPortFeaturesFlags advertised) {
    advertised_ = advertised;
  }

  OFPPortFeaturesFlags supported() const { return supported_; }
  void setSupported(OFPPortFeaturesFlags supported) { supported_ = supported; }

  OFPPortFeaturesFlags peer() const { return peer_; }
  void setPeer(OFPPortFeaturesFlags peer) { peer_ = peer; }

  UInt32 currSpeed() const { return currSpeed_; }
  void setCurrSpeed(UInt32 currSpeed) { currSpeed_ = currSpeed; }

  UInt32 maxSpeed() const { return maxSpeed_; }
  void setMaxSpeed(UInt32 maxSpeed) { maxSpeed_ = maxSpeed; }

 private:
  PortNumber portNo_;
  Padding<4> pad1_;
  MacAddress hwAddr_;
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
};

static_assert(sizeof(PortV2) == 64, "Unexpected size.");
static_assert(IsStandardLayout<PortV2>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PortV2>(), "Expected trivially copyable.");

}  // namespace deprecated
}  // namespace ofp

#endif  // OFP_PORT_H_
