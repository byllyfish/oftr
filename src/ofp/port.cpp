// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/port.h"
#include "ofp/portproperty.h"
#include "ofp/validation.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange Port::properties() const {
  return SafeByteRange(this, length_, sizeof(Port));
}

bool Port::validateInput(Validation *context) const {
  size_t remaining = context->lengthRemaining();
  if (!context->validateBool(remaining >= sizeof(Port),
                             "Length remaining too short for Port")) {
    return false;
  }

  size_t len = length_;
  if (!context->validateBool(len <= remaining && len >= sizeof(Port),
                             "Invalid length for Port")) {
    return false;
  }

  return true;
}

PortBuilder::PortBuilder(const deprecated::PortV1 &port) {
  setPortNo(PortNumber::fromV1(port.portNo()));
  setHwAddr(port.hwAddr());
  setName(port.name());
  setConfig(port.config());
  setState(port.state());

  PortPropertyEthernet prop;
  prop.setCurr(port.curr());
  prop.setAdvertised(port.advertised());
  prop.setSupported(port.supported());
  prop.setPeer(port.peer());
  prop.setCurrSpeed(0);
  prop.setMaxSpeed(0);
  properties_.add(prop);

  updateLen();
}

PortBuilder::PortBuilder(const deprecated::PortV2 &port) {
  setPortNo(port.portNo());
  setHwAddr(port.hwAddr());
  setName(port.name());
  setConfig(port.config());
  setState(port.state());

  PortPropertyEthernet prop;
  prop.setCurr(port.curr());
  prop.setAdvertised(port.advertised());
  prop.setSupported(port.supported());
  prop.setPeer(port.peer());
  prop.setCurrSpeed(port.currSpeed());
  prop.setMaxSpeed(port.maxSpeed());
  properties_.add(prop);

  updateLen();
}

size_t PortBuilder::writeSize(Writable *channel) {
  UInt8 version = channel->version();

  if (version >= OFP_VERSION_5)
    return sizeof(msg_) + properties_.size();
  if (version >= OFP_VERSION_2)
    return sizeof(deprecated::PortV2);
  return sizeof(deprecated::PortV1);
}

void PortBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version >= OFP_VERSION_5) {
    assert(msg_.length_ == SizeWithoutProperties + properties_.size());
    channel->write(&msg_, sizeof(msg_));
    channel->write(properties_.data(), properties_.size());
  } else if (version >= OFP_VERSION_2) {
    deprecated::PortV2 port{*this};
    channel->write(&port, sizeof(port));
  } else {
    deprecated::PortV1 port{*this};
    channel->write(&port, sizeof(port));
  }

  // FIXME(bfish): Do we want this here?
  channel->flush();
}

void PortBuilder::copyTo(UInt8 *ptr) {
  std::memcpy(ptr, &msg_, sizeof(msg_));
  ptr += sizeof(msg_);
  std::memcpy(ptr, properties_.data(), properties_.size());
}

PortBuilder &PortBuilder::operator=(const PortBuilder &port) {
  if (&port != this) {
    std::memcpy(&msg_, &port.msg_, sizeof(port.msg_));
    properties_ = port.properties_;
  }
  return *this;
}

deprecated::PortV1::PortV1(const Port &port) {
  portNo_ = UInt16_narrow_cast(port.portNo());
  hwAddr_ = port.hwAddr();
  name_ = port.name();
  config_ = port.config();
  state_ = port.state();

  PropertyRange props = port.properties();
  auto iter = props.findProperty(PortPropertyEthernet::type());

  if (iter != props.end()) {
    const PortPropertyEthernet &eth = iter->property<PortPropertyEthernet>();

    // Need to be converted.
    setCurr(eth.curr());
    setAdvertised(eth.advertised());
    setSupported(eth.supported());
    setPeer(eth.peer());

  } else {
    setCurr(OFPPF_NONE);
    setAdvertised(OFPPF_NONE);
    setSupported(OFPPF_NONE);
    setPeer(OFPPF_NONE);
  }
}

deprecated::PortV1::PortV1(const PortBuilder &portBuilder) {
  const Port &port = portBuilder.msg_;

  portNo_ = UInt16_narrow_cast(port.portNo());
  hwAddr_ = port.hwAddr();
  name_ = port.name();
  config_ = port.config();
  state_ = port.state();

  PropertyRange props = portBuilder.properties_.toRange();
  auto iter = props.findProperty(PortPropertyEthernet::type());

  if (iter != props.end()) {
    const PortPropertyEthernet &eth = iter->property<PortPropertyEthernet>();

    // Need to be converted.
    setCurr(eth.curr());
    setAdvertised(eth.advertised());
    setSupported(eth.supported());
    setPeer(eth.peer());

  } else {
    setCurr(OFPPF_NONE);
    setAdvertised(OFPPF_NONE);
    setSupported(OFPPF_NONE);
    setPeer(OFPPF_NONE);
  }
}

deprecated::PortV2::PortV2(const Port &port) {
  portNo_ = port.portNo();
  hwAddr_ = port.hwAddr();
  name_ = port.name();
  config_ = port.config();
  state_ = port.state();

  PropertyRange props = port.properties();
  auto iter = props.findProperty(PortPropertyEthernet::type());

  if (iter != props.end()) {
    const PortPropertyEthernet &eth = iter->property<PortPropertyEthernet>();

    setCurr(eth.curr());
    setAdvertised(eth.advertised());
    setSupported(eth.supported());
    setPeer(eth.peer());
    setCurrSpeed(eth.currSpeed());
    setMaxSpeed(eth.maxSpeed());

  } else {
    setCurr(OFPPF_NONE);
    setAdvertised(OFPPF_NONE);
    setSupported(OFPPF_NONE);
    setPeer(OFPPF_NONE);
    setCurrSpeed(0);
    setMaxSpeed(0);
  }
}

deprecated::PortV2::PortV2(const PortBuilder &portBuilder) {
  const Port &port = portBuilder.msg_;

  portNo_ = port.portNo();
  hwAddr_ = port.hwAddr();
  name_ = port.name();
  config_ = port.config();
  state_ = port.state();

  PropertyRange props = portBuilder.properties_.toRange();
  auto iter = props.findProperty(PortPropertyEthernet::type());

  if (iter != props.end()) {
    const PortPropertyEthernet &eth = iter->property<PortPropertyEthernet>();

    setCurr(eth.curr());
    setAdvertised(eth.advertised());
    setSupported(eth.supported());
    setPeer(eth.peer());
    setCurrSpeed(eth.currSpeed());
    setMaxSpeed(eth.maxSpeed());

  } else {
    setCurr(OFPPF_NONE);
    setAdvertised(OFPPF_NONE);
    setSupported(OFPPF_NONE);
    setPeer(OFPPF_NONE);
    setCurrSpeed(0);
    setMaxSpeed(0);
  }
}
