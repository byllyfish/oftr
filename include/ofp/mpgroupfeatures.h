// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPGROUPFEATURES_H_
#define OFP_MPGROUPFEATURES_H_

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp {

class Writable;
class Validation;

class MPGroupFeatures {
 public:
  OFPGroupTypeFlags types() const { return types_; }
  OFPGroupCapabilityFlags capabilities() const { return capabilities_; }

  OFPActionTypeFlags actionsALL() const { return actions_[0]; }
  OFPActionTypeFlags actionsSEL() const { return actions_[1]; }
  OFPActionTypeFlags actionsIND() const { return actions_[2]; }
  OFPActionTypeFlags actionsFF() const { return actions_[3]; }

  bool validateInput(Validation *context) const;

 private:
  Big<OFPGroupTypeFlags> types_;
  Big<OFPGroupCapabilityFlags> capabilities_;
  Big32 maxGroups_[4];
  Big<OFPActionTypeFlags> actions_[4];

  friend class MPGroupFeaturesBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(OFPActionTypeFlags) == 4, "Unexpected size.");
static_assert(sizeof(MPGroupFeatures) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPGroupFeatures>(), "Expected standard layout.");

class MPGroupFeaturesBuilder {
 public:
  MPGroupFeaturesBuilder() = default;

  void setTypes(OFPGroupTypeFlags types) { msg_.types_ = types; }
  void setCapabilities(OFPGroupCapabilityFlags capabilities) {
    msg_.capabilities_ = capabilities;
  }

  void setActionsALL(OFPActionTypeFlags actions) { msg_.actions_[0] = actions; }
  void setActionsSEL(OFPActionTypeFlags actions) { msg_.actions_[1] = actions; }
  void setActionsIND(OFPActionTypeFlags actions) { msg_.actions_[2] = actions; }
  void setActionsFF(OFPActionTypeFlags actions) { msg_.actions_[3] = actions; }

  void write(Writable *channel);
  void reset() {}

 private:
  MPGroupFeatures msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPGROUPFEATURES_H_
