// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/port.h"
#include "ofp/writable.h"

namespace ofp {

PortBuilder::PortBuilder(const deprecated::PortV1 &port) {
  setPortNo(port.portNo());
  setHwAddr(port.hwAddr());
  setName(port.name());
  setConfig(port.config());
  setState(port.state());
  setCurr(port.curr());
  setAdvertised(port.advertised());
  setSupported(port.supported());
  setPeer(port.peer());
  setCurrSpeed(0);
  setMaxSpeed(0);
}

void PortBuilder::write(Writable *channel) {
  channel->write(&msg_, sizeof(msg_));
  channel->flush();
}

namespace deprecated {

PortV1::PortV1(const Port &port) {
  portNo_ = UInt16_narrow_cast(port.portNo());
  hwAddr_ = port.hwAddr();
  name_ = port.name();
  config_ = port.config();
  state_ = port.state();
  curr_ = port.curr();
  advertised_ = port.advertised();
  supported_ = port.supported();
  peer_ = port.peer();
}

}  // namespace deprecated
}  // namespace ofp
