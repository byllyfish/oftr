#ifndef OFP_MPGROUPFEATURES_H_
#define OFP_MPGROUPFEATURES_H_

#include "ofp/byteorder.h"

namespace ofp {

class Writable;
class Validation;

class MPGroupFeatures {
 public:
  UInt32 types() const { return types_; }

  bool validateInput(Validation *context) const;

 private:
  Big32 types_;
  Big32 capabilities_;
  Big32 maxGroups_[4];
  Big32 actions_[4];

  friend class MPGroupFeaturesBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPGroupFeatures) == 40, "Unexpected size.");
static_assert(IsStandardLayout<MPGroupFeatures>(), "Expected standard layout.");

class MPGroupFeaturesBuilder {
 public:
  MPGroupFeaturesBuilder() = default;

  void write(Writable *channel);
  void reset() {}

 private:
  MPGroupFeatures msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPGROUPFEATURES_H_
