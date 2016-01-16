// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SETCONFIG_H_
#define OFP_SETCONFIG_H_

#include "ofp/protocolmsg.h"
#include "ofp/controllermaxlen.h"

namespace ofp {

class SetConfig
    : public ProtocolMsg<SetConfig, OFPT_SET_CONFIG, 12, 12, false> {
 public:
  OFPConfigFlags flags() const { return flags_; }
  ControllerMaxLen missSendLen() const { return missSendLen_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big<OFPConfigFlags> flags_ = OFPC_FRAG_NORMAL;
  ControllerMaxLen missSendLen_ = 128;

  // Only SetConfigBuilder can construct an instance.
  SetConfig() : header_{type()} {}

  friend class SetConfigBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(SetConfig) == 12, "Unexpected size.");
static_assert(IsStandardLayout<SetConfig>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<SetConfig>(), "Expected trivially copyable.");

class SetConfigBuilder {
 public:
  SetConfigBuilder() = default;
  explicit SetConfigBuilder(const SetConfig *msg);

  void setFlags(OFPConfigFlags flags) { msg_.flags_ = flags; }
  void setMissSendLen(ControllerMaxLen missSendLen) {
    msg_.missSendLen_ = missSendLen;
  }

  UInt32 send(Writable *channel);

 private:
  SetConfig msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_SETCONFIG_H_
