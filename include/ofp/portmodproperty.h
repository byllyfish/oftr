// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PORTMODPROPERTY_H_
#define OFP_PORTMODPROPERTY_H_

#include "ofp/experimenterproperty.h"

namespace ofp {

class PortModPropertyEthernet : private NonCopyable {
 public:
  constexpr static OFPPortModProperty type() { return OFPPMPT_ETHERNET; }

  using ValueType = OFPPortFeaturesFlags;

  OFPPortFeaturesFlags advertise() const { return advertise_; }
  void setAdvertise(OFPPortFeaturesFlags advertise) { advertise_ = advertise; }

  OFPPortFeaturesFlags value() const { return advertise_; }
  static OFPPortFeaturesFlags defaultValue() { return OFPPF_NONE; }

 private:
  Big16 type_ = type();
  Big16 len_ = 8;
  Big<OFPPortFeaturesFlags> advertise_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortModPropertyEthernet) == 8, "Unexpected size.");

// PortModPropertyOptical needs to be `Copyable` since it is used as an
// optional property.

class PortModPropertyOptical {
 public:
  constexpr static OFPPortModProperty type() { return OFPPMPT_OPTICAL; }

  OFPOpticalPortFeaturesFlags configure() const { return configure_; }
  UInt32 freqLmda() const { return freqLmda_; }
  SignedInt32 flOffset() const { return flOffset_; }
  UInt32 gridSpan() const { return gridSpan_; }
  UInt32 txPwr() const { return txPwr_; }

  void setConfigure(OFPOpticalPortFeaturesFlags configure) {
    configure_ = configure;
  }
  void setFreqLmda(UInt32 freqLmda) { freqLmda_ = freqLmda; }
  void setFlOffset(SignedInt32 flOffset) { flOffset_ = flOffset; }
  void setGridSpan(UInt32 gridSpan) { gridSpan_ = gridSpan; }
  void setTxPwr(UInt32 txPwr) { txPwr_ = txPwr; }

 private:
  Big16 type_ = type();
  Big16 len_ = 24;
  Big<OFPOpticalPortFeaturesFlags> configure_;
  Big32 freqLmda_;
  SignedBig32 flOffset_;
  Big32 gridSpan_;
  Big32 txPwr_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortModPropertyOptical) == 24, "Unexpected size.");

using PortModPropertyExperimenter =
    detail::ExperimenterProperty<OFPPortModProperty, OFPPMPT_EXPERIMENTER>;

}  // namespace ofp

#endif  // OFP_PORTMODPROPERTY_H_
