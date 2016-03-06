// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PORTSTATUS_H_
#define OFP_PORTSTATUS_H_

#include "ofp/port.h"
#include "ofp/protocolmsg.h"

namespace ofp {

class PortStatus : public ProtocolMsg<PortStatus, OFPT_PORT_STATUS, 56> {
 public:
  OFPPortStatusReason reason() const { return reason_; }

  const Port &port() const { return *Interpret_cast<Port>(BytePtr(this) + 16); }

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big<OFPPortStatusReason> reason_;
  Padding<7> pad_;

  // Only PortStatusBuilder can create an instance.
  PortStatus() : header_{type()} {}

  friend class PortStatusBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortStatus) == 16, "Unexpected size.");
static_assert(IsStandardLayout<PortStatus>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<PortStatus>(),
              "Expected trivially copyable.");

class PortStatusBuilder {
 public:
  PortStatusBuilder() = default;
  explicit PortStatusBuilder(const PortStatus *msg) : msg_{*msg} {}

  void setReason(OFPPortStatusReason reason) { msg_.reason_ = reason; }
  void setPort(const PortBuilder &port) { port_ = port; }

  UInt32 send(Writable *channel);

 private:
  PortStatus msg_;
  PortBuilder port_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_PORTSTATUS_H_
