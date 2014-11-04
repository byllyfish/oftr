// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_PORT_H_
#define OFP_PORT_H_

#include "ofp/byteorder.h"
#include "ofp/strings.h"
#include "ofp/padding.h"
#include "ofp/enetaddress.h"

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

  UInt32 portNo() const { return portNo_; }
  const EnetAddress &hwAddr() const { return hwAddr_; }
  const PortNameStr &name() const { return name_; }
  UInt32 config() const { return config_; }
  UInt32 state() const { return state_; }
  UInt32 curr() const { return curr_; }
  UInt32 advertised() const { return advertised_; }
  UInt32 supported() const { return supported_; }
  UInt32 peer() const { return peer_; }
  UInt32 currSpeed() const { return currSpeed_; }
  UInt32 maxSpeed() const { return maxSpeed_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Big32 portNo_;
  Padding<4> pad1_;
  EnetAddress hwAddr_;
  Padding<2> pad2_;
  PortNameStr name_;
  Big32 config_;
  Big32 state_;
  Big32 curr_;
  Big32 advertised_;
  Big32 supported_;
  Big32 peer_;
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

  void setPortNo(UInt32 portNo) { msg_.portNo_ = portNo; }
  void setHwAddr(const EnetAddress &hwAddr) { msg_.hwAddr_ = hwAddr; }
  void setName(const PortNameStr &name) { msg_.name_ = name; }
  void setConfig(UInt32 config) { msg_.config_ = config; }
  void setState(UInt32 state) { msg_.state_ = state; }
  void setCurr(UInt32 curr) { msg_.curr_ = curr; }
  void setAdvertised(UInt32 advertised) { msg_.advertised_ = advertised; }
  void setSupported(UInt32 supported) { msg_.supported_ = supported; }
  void setPeer(UInt32 peer) { msg_.peer_ = peer; }
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

  UInt32 config() const { return config_; }
  void setConfig(UInt32 config) { config_ = config; }

  UInt32 state() const { return state_; }
  void setState(UInt32 state) { state_ = state; }

  UInt32 curr() const { return curr_; }
  void setCurr(UInt32 curr) { curr_ = curr; }

  UInt32 advertised() const { return advertised_; }
  void setAdvertised(UInt32 advertised) { advertised_ = advertised; }

  UInt32 supported() const { return supported_; }
  void setSupported(UInt32 supported) { supported_ = supported; }

  UInt32 peer() const { return peer_; }
  void setPeer(UInt32 peer) { peer_ = peer; }

 private:
  Big16 portNo_;
  EnetAddress hwAddr_;
  PortNameStr name_;
  Big32 config_;
  Big32 state_;
  Big32 curr_;
  Big32 advertised_;
  Big32 supported_;
  Big32 peer_;
};

static_assert(sizeof(PortV1) == 48, "Unexpected size.");
static_assert(IsStandardLayout<PortV1>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PortV1>(), "Expected trivially copyable.");

}  // namespace deprecated
}  // namespace ofp

#endif  // OFP_PORT_H_
