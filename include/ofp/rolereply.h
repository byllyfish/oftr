// Copyright 2014-present Bill Fisher. All rights reserved.

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

  bool validateInput(Validation *context) const { return true; }

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
