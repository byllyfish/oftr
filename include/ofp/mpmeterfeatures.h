// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPMETERFEATURES_H_
#define OFP_MPMETERFEATURES_H_

#include "ofp/byteorder.h"
#include "ofp/constants.h"
#include "ofp/padding.h"

namespace ofp {

class Validation;

class MPMeterFeatures {
 public:
  UInt32 maxMeter() const { return maxMeter_; }
  OFPMeterBandFlags bandTypes() const { return bandTypes_; }
  OFPMeterFlags capabilities() const { return capabilities_; }
  UInt8 maxBands() const { return maxBands_; }
  UInt8 maxColor() const { return maxColor_; }

  bool validateInput(Validation *context) const;

 private:
  Big32 maxMeter_;
  Big<OFPMeterBandFlags> bandTypes_;
  Big<OFPMeterFlags> capabilities_;
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
  void setBandTypes(OFPMeterBandFlags bandTypes) {
    msg_.bandTypes_ = bandTypes;
  }
  void setCapabilities(OFPMeterFlags capabilities) {
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
