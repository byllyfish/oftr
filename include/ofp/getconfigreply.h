// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_GETCONFIGREPLY_H_
#define OFP_GETCONFIGREPLY_H_

#include "ofp/protocolmsg.h"

namespace ofp {

class GetConfigReply
    : public ProtocolMsg<GetConfigReply, OFPT_GET_CONFIG_REPLY, 12, 12, false> {
 public:
  OFPConfigFlags flags() const { return flags_; }
  UInt16 missSendLen() const { return missSendLen_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big<OFPConfigFlags> flags_;
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

  void setFlags(OFPConfigFlags flags) { msg_.flags_ = flags; }
  void setMissSendLen(UInt16 missSendLen) { msg_.missSendLen_ = missSendLen; }

  UInt32 send(Writable *channel);

 private:
  GetConfigReply msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_GETCONFIGREPLY_H_
