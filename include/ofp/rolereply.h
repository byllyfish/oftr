//  ===== ---- ofp/rolereply.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the RoleReply and RoleReplyBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ROLEREPLY_H_
#define OFP_ROLEREPLY_H_

#include "ofp/protocolmsg.h"
#include "ofp/padding.h"
#include "ofp/rolerequest.h"

namespace ofp {

// FIXME- this is identical to RoleRequest - reuse with template?

class RoleReply : public ProtocolMsg<RoleReply, OFPT_ROLE_REPLY, 24, 24> {
public:
  UInt32 role() const { return role_; }
  UInt64 generationId() const { return generationId_; }

  bool validateLength(size_t length) const;

private:
  Header header_;
  Big32 role_;
  Padding<4> pad_;
  Big64 generationId_;

  // Only RoleReplyBuilder can construct an instance.
  RoleReply() : header_{type()} {}

  friend class RoleReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(RoleReply) == 24, "Unexpected size.");
static_assert(IsStandardLayout<RoleReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<RoleReply>(), "Expected trivially copyable.");

class RoleReplyBuilder {
public:
  explicit RoleReplyBuilder(UInt32 xid);
  explicit RoleReplyBuilder(const RoleRequest *request);
  explicit RoleReplyBuilder(const RoleReply *msg);

  void setRole(UInt32 role) { msg_.role_ = role; }
  void setGenerationId(UInt64 generationId) {
    msg_.generationId_ = generationId;
  }

  UInt32 send(Writable *channel);

private:
  RoleReply msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_ROLEREPLY_H_
