// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ROLEREQUEST_H_
#define OFP_ROLEREQUEST_H_

#include "ofp/padding.h"
#include "ofp/protocolmsg.h"

namespace ofp {

class RoleRequest : public ProtocolMsg<RoleRequest, OFPT_ROLE_REQUEST, 24, 24> {
 public:
  OFPControllerRole role() const { return role_; }
  UInt64 generationId() const { return generationId_; }

  UInt32 xid() const { return header_.xid(); }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big<OFPControllerRole> role_;
  Padding<4> pad_;
  Big64 generationId_;

  // Only RoleRequestBuilder can construct an instance.
  RoleRequest() : header_{type()} {}

  friend class RoleRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(RoleRequest) == 24, "Unexpected size.");
static_assert(IsStandardLayout<RoleRequest>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<RoleRequest>(),
              "Expected trivially copyable.");

class RoleRequestBuilder {
 public:
  RoleRequestBuilder() = default;
  explicit RoleRequestBuilder(const RoleRequest *msg);

  void setRole(OFPControllerRole role) { msg_.role_ = role; }
  void setGenerationId(UInt64 generationId) {
    msg_.generationId_ = generationId;
  }

  UInt32 send(Writable *channel);

 private:
  RoleRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_ROLEREQUEST_H_
