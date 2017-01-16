// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_REQUESTFORWARD_H_
#define OFP_REQUESTFORWARD_H_

#include "ofp/protocolmsg.h"

namespace ofp {

class RequestForward : public ProtocolMsg<RequestForward, OFPT_REQUESTFORWARD,
                                          16, 65535, false> {
 public:
  ByteRange request() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;

  // Only RequestForwardBuilder can construct an instance.
  RequestForward() : header_{type()} {}

  friend class RequestForwardBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(RequestForward) == 8, "Unexpected size.");
static_assert(IsStandardLayout<RequestForward>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<RequestForward>(),
              "Expected trivially copyable.");

class RequestForwardBuilder {
 public:
  void setRequest(const ByteList &request) { request_ = request; }

  UInt32 send(Writable *channel);

 private:
  RequestForward msg_;
  ByteList request_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_REQUESTFORWARD_H_
