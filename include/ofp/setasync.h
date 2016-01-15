// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SETASYNC_H_
#define OFP_SETASYNC_H_

#include "ofp/protocolmsg.h"
#include "ofp/propertylist.h"

namespace ofp {

class SetAsync : public ProtocolMsg<SetAsync, OFPT_SET_ASYNC, 8, 65528> {
 public:
  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;

  // Only SetAsyncBuilder can construct an instance.
  SetAsync() : header_{type()} {}

  friend class SetAsyncBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(SetAsync) == 8, "Unexpected size.");
static_assert(IsStandardLayout<SetAsync>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<SetAsync>(), "Expected trivially copyable.");

class SetAsyncBuilder {
 public:
  SetAsyncBuilder() = default;
  explicit SetAsyncBuilder(const SetAsync *msg);

  void setProperties(const PropertyList &properties) {
    properties_ = properties;
  }

  UInt32 send(Writable *channel);

 private:
  SetAsync msg_;
  PropertyList properties_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_SETASYNC_H_
