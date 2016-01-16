// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BUNDLECONTROL_H_
#define OFP_BUNDLECONTROL_H_

#include "ofp/protocolmsg.h"
#include "ofp/propertylist.h"

namespace ofp {

class Writable;

class BundleControl
    : public ProtocolMsg<BundleControl, OFPT_BUNDLE_CONTROL, 16, 65528> {
 public:
  UInt32 bundleId() const { return bundleId_; }
  OFPBundleCtrlType ctrlType() const { return ctrlType_; }
  OFPBundleFlags flags() const { return flags_; }
  PropertyRange properties() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big32 bundleId_;
  Big<OFPBundleCtrlType> ctrlType_;
  Big<OFPBundleFlags> flags_;

  // Only BundleControlBuilder can construct an instance.
  BundleControl() : header_{type()} {}

  friend class BundleControlBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(BundleControl) == 16, "Unexpected size.");
static_assert(IsStandardLayout<BundleControl>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<BundleControl>(),
              "Expected trivially copyable.");

class BundleControlBuilder {
 public:
  void setBundleId(UInt32 bundleId) { msg_.bundleId_ = bundleId; }
  void setCtrlType(OFPBundleCtrlType ctrlType) { msg_.ctrlType_ = ctrlType; }
  void setFlags(OFPBundleFlags flags) { msg_.flags_ = flags; }
  void setProperties(const PropertyList &properties) {
    properties_ = properties;
  }

  UInt32 send(Writable *channel);

 private:
  BundleControl msg_;
  PropertyList properties_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_BUNDLECONTROL_H_
