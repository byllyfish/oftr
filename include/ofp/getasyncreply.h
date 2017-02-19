// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_GETASYNCREPLY_H_
#define OFP_GETASYNCREPLY_H_

#include "ofp/headeronly.h"
#include "ofp/propertylist.h"
#include "ofp/protocolmsg.h"

namespace ofp {

class GetAsyncReply
    : public ProtocolMsg<GetAsyncReply, OFPT_GET_ASYNC_REPLY, 8> {
 public:
  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;

  // Only GetAsyncReplyBuilder can construct an instance.
  GetAsyncReply() : header_{type()} {}

  friend class GetAsyncReplyBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(GetAsyncReply) == 8, "Unexpected size.");
static_assert(IsStandardLayout<GetAsyncReply>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<GetAsyncReply>(),
              "Expected trivially copyable.");

class GetAsyncReplyBuilder {
 public:
  explicit GetAsyncReplyBuilder(UInt32 xid);
  explicit GetAsyncReplyBuilder(const GetAsyncRequest *request);
  explicit GetAsyncReplyBuilder(const GetAsyncReply *msg);

  void setProperties(const PropertyList &properties) {
    properties_ = properties;
  }

  UInt32 send(Writable *channel);

 private:
  GetAsyncReply msg_;
  PropertyList properties_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_GETASYNCREPLY_H_
