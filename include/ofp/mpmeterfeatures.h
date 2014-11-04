// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPMETERFEATURES_H_
#define OFP_MPMETERFEATURES_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class Validation;

class MPMeterFeatures {
 public:
  UInt32 maxMeter() const { return maxMeter_; }
  UInt32 bandTypes() const { return bandTypes_; }
  UInt32 capabilities() const { return capabilities_; }
  UInt8 maxBands() const { return maxBands_; }
  UInt8 maxColor() const { return maxColor_; }

  bool validateInput(Validation *context) const;

 private:
  Big32 maxMeter_;
  Big32 bandTypes_;
  Big32 capabilities_;
  Big8 maxBands_;
  Big8 maxColor_;
  Padding<2> pad_;

  friend class MPMeterFeaturesBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPMeterFeatures) == 16, "Unexpected size.");
static_assert(IsStandardLayout<MPMeterFeatures>(), "Expected standard layout.");

class MPMeterFeaturesBuilder {
 public:
  void setMaxMeter(UInt32 maxMeter) { msg_.maxMeter_ = maxMeter; }
  void setBandTypes(UInt32 bandTypes) { msg_.bandTypes_ = bandTypes; }
  void setCapabilities(UInt32 capabilities) {
    msg_.capabilities_ = capabilities;
  }
  void setMaxBands(UInt8 maxBands) { msg_.maxBands_ = maxBands; }
  void setMaxColor(UInt8 maxColor) { msg_.maxColor_ = maxColor; }

 private:
  MPMeterFeatures msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPMETERFEATURES_H_
