//  ===== ---- ofp/setconfig.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the SetConfig and SetConfigBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SETCONFIG_H_
#define OFP_SETCONFIG_H_

#include "ofp/protocolmsg.h"

namespace ofp {

class SetConfig
    : public ProtocolMsg<SetConfig, OFPT_SET_CONFIG, 12, 12, false> {
public:
  UInt16 flags() const { return flags_; }
  UInt16 missSendLen() const { return missSendLen_; }

  bool validateInput(size_t length) const;

private:
  Header header_;
  Big16 flags_ = 0;
  Big16 missSendLen_ = 128;

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

  void setFlags(UInt16 flags);
  void setMissSendLen(UInt16 missSendLen);

  UInt32 send(Writable *channel);

private:
  SetConfig msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_SETCONFIG_H_
