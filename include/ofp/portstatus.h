//  ===== ---- ofp/portstatus.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the PortStatus and PortStatusBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PORTSTATUS_H_
#define OFP_PORTSTATUS_H_

#include "ofp/protocolmsg.h"
#include "ofp/port.h"

namespace ofp {

class PortStatus
    : public ProtocolMsg<PortStatus, OFPT_PORT_STATUS, 80, 80, false> {
public:
  UInt8 reason() const { return reason_; }

  const Port &port() const { return port_; }

  bool validateInput(Validation *context) const { return true; }

private:
  Header header_;
  UInt8 reason_;
  Padding<7> pad_;
  Port port_;

  // Only PortStatusBuilder can create an instance.
  PortStatus() : header_{type()} {}

  friend class PortStatusBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortStatus) == 80, "Unexpected size.");
static_assert(IsStandardLayout<PortStatus>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PortStatus>(),
              "Expected trivially copyable.");

class PortStatusBuilder {
public:
  PortStatusBuilder() = default;
  explicit PortStatusBuilder(const PortStatus *msg);

  void setReason(UInt8 reason) { msg_.reason_ = reason; }
  void setPort(const PortBuilder &port) { msg_.port_ = port.toPort(); }

  UInt32 send(Writable *channel);

private:
  PortStatus msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PORTSTATUS_H_
