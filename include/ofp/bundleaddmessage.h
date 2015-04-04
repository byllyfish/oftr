// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_BUNDLEADDMESSAGE_H_
#define OFP_BUNDLEADDMESSAGE_H_

#include "ofp/protocolmsg.h"
#include "ofp/propertylist.h"

namespace ofp {

class Writable;

class BundleAddMessage
    : public ProtocolMsg<BundleAddMessage, OFPT_BUNDLE_ADD_MESSAGE, 24, 65528> {
 public:
  UInt32 bundleId() const { return bundleId_; }
  UInt16 flags() const { return flags_; }

  ByteRange message() const;
  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big32 bundleId_;
  Padding<2> pad_;
  Big16 flags_;

  // Only BundleAddMessageBuilder can construct an instance.
  BundleAddMessage() : header_{type()} {}

  friend class BundleAddMessageBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(BundleAddMessage) == 16, "Unexpected size.");
static_assert(IsStandardLayout<BundleAddMessage>(),
              "Expected standard layout.");
static_assert(IsTriviallyCopyable<BundleAddMessage>(),
              "Expected trivially copyable.");

class BundleAddMessageBuilder {
 public:
  void setBundleId(UInt32 bundleId) { msg_.bundleId_ = bundleId; }
  void setFlags(UInt16 flags) { msg_.flags_ = flags; }

  void setMessage(const ByteList &message) { message_ = message; }
  void setProperties(const PropertyList &properties) {
    properties_ = properties;
  }

  UInt32 send(Writable *channel);

 private:
  BundleAddMessage msg_;
  ByteList message_;
  PropertyList properties_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_BUNDLEADDMESSAGE_H_
