//  ===== ---- ofp/getconfigreply.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines the GetConfigReply and GetConfigReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_GETCONFIGREPLY_H_
#define OFP_GETCONFIGREPLY_H_

#include "ofp/protocolmsg.h"

namespace ofp {

class GetConfigReply
    : public ProtocolMsg<GetConfigReply, OFPT_GET_CONFIG_REPLY, 12, 12, false> {
 public:
  UInt16 flags() const { return flags_; }
  UInt16 missSendLen() const { return missSendLen_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big16 flags_;
  Big16 missSendLen_;

  // Only GetConfigReplyBuilder can construct an instance.
  GetConfigReply() : header_{type()} {}

  friend class GetConfigReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(GetConfigReply) == 12, "Unexpected size.");
static_assert(IsStandardLayout<GetConfigReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<GetConfigReply>(),
              "Expected trivially copyable.");

class GetConfigReplyBuilder {
 public:
  GetConfigReplyBuilder() = default;
  explicit GetConfigReplyBuilder(const GetConfigReply *msg);

  void setFlags(UInt16 flags);
  void setMissSendLen(UInt16 missSendLen);

  UInt32 send(Writable *channel);

 private:
  GetConfigReply msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_GETCONFIGREPLY_H_
