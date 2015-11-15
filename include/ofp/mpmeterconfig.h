// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPMETERCONFIG_H_
#define OFP_MPMETERCONFIG_H_

#include "ofp/meterbandlist.h"
#include "ofp/meternumber.h"

namespace ofp {

class Writable;

class MPMeterConfig {
 public:
  enum { MPVariableSizeOffset = 0 };

  OFPMeterConfigFlags flags() const { return flags_; }
  MeterNumber meterId() const { return meterId_; }

  MeterBandRange bands() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 length_;
  Big<OFPMeterConfigFlags> flags_;
  MeterNumber meterId_;

  friend class MPMeterConfigBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPMeterConfig) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPMeterConfig>(), "Expected standard layout.");

class MPMeterConfigBuilder {
 public:
  void setFlags(OFPMeterConfigFlags flags) { msg_.flags_ = flags; }
  void setMeterId(MeterNumber meterId) { msg_.meterId_ = meterId; }

  void write(Writable *channel);
  void reset() { bands_.clear(); }

 private:
  MPMeterConfig msg_;
  MeterBandList bands_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPMETERCONFIG_H_
