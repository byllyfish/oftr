//  ===== ---- ofp/port.cpp --------------------------------*- C++ -*- =====  //
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
/// \brief Implements Port class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/port.h"

namespace ofp { // <namespace ofp>

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

namespace deprecated { // <namespace deprecated>

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

} // </namespace deprecated>
} // </namespace ofp>
