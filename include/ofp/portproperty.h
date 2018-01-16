// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PORTPROPERTY_H_
#define OFP_PORTPROPERTY_H_

#include "ofp/experimenterproperty.h"

namespace ofp {

class PortPropertyEthernet : private NonCopyable {
 public:
  constexpr static OFPPortProperty type() { return OFPPDPT_ETHERNET; }

  OFPPortFeaturesFlags curr() const { return curr_; }
  OFPPortFeaturesFlags advertised() const { return advertised_; }
  OFPPortFeaturesFlags supported() const { return supported_; }
  OFPPortFeaturesFlags peer() const { return peer_; }
  UInt32 currSpeed() const { return currSpeed_; }
  UInt32 maxSpeed() const { return maxSpeed_; }

  void setCurr(OFPPortFeaturesFlags curr) { curr_ = curr; }
  void setAdvertised(OFPPortFeaturesFlags advertised) {
    advertised_ = advertised;
  }
  void setSupported(OFPPortFeaturesFlags supported) { supported_ = supported; }
  void setPeer(OFPPortFeaturesFlags peer) { peer_ = peer; }
  void setCurrSpeed(UInt32 currSpeed) { currSpeed_ = currSpeed; }
  void setMaxSpeed(UInt32 maxSpeed) { maxSpeed_ = maxSpeed; }

 private:
  Big16 type_ = type();
  Big16 len_ = 32;
  Padding<4> pad_;
  Big<OFPPortFeaturesFlags> curr_;
  Big<OFPPortFeaturesFlags> advertised_;
  Big<OFPPortFeaturesFlags> supported_;
  Big<OFPPortFeaturesFlags> peer_;
  Big32 currSpeed_;
  Big32 maxSpeed_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortPropertyEthernet) == 32, "Unexpected size.");

// PortPropertyOptical has to be `Copyable` since it is used as an optional
// property.

class PortPropertyOptical {
 public:
  constexpr static OFPPortProperty type() { return OFPPDPT_OPTICAL; }

  OFPOpticalPortFeaturesFlags supported() const { return supported_; }
  UInt32 txMinFreqLmda() const { return txMinFreqLmda_; }
  UInt32 txMaxFreqLmda() const { return txMaxFreqLmda_; }
  UInt32 txGridFreqLmda() const { return txGridFreqLmda_; }
  UInt32 rxMinFreqLmda() const { return rxMinFreqLmda_; }
  UInt32 rxMaxFreqLmda() const { return rxMaxFreqLmda_; }
  UInt32 rxGridFreqLmda() const { return rxGridFreqLmda_; }
  UInt16 txPwrMin() const { return txPwrMin_; }
  UInt16 txPwrMax() const { return txPwrMax_; }

  void setSupported(OFPOpticalPortFeaturesFlags supported) {
    supported_ = supported;
  }
  void setTxMinFreqLmda(UInt32 txMinFreqLmda) {
    txMinFreqLmda_ = txMinFreqLmda;
  }
  void setTxMaxFreqLmda(UInt32 txMaxFreqLmda) {
    txMaxFreqLmda_ = txMaxFreqLmda;
  }
  void setTxGridFreqLmda(UInt32 txGridFreqLmda) {
    txGridFreqLmda_ = txGridFreqLmda;
  }
  void setRxMinFreqLmda(UInt32 rxMinFreqLmda) {
    rxMinFreqLmda_ = rxMinFreqLmda;
  }
  void setRxMaxFreqLmda(UInt32 rxMaxFreqLmda) {
    rxMaxFreqLmda_ = rxMaxFreqLmda;
  }
  void setRxGridFreqLmda(UInt32 rxGridFreqLmda) {
    rxGridFreqLmda_ = rxGridFreqLmda;
  }
  void setTxPwrMin(UInt16 txPwrMin) { txPwrMin_ = txPwrMin; }
  void setTxPwrMax(UInt16 txPwrMax) { txPwrMax_ = txPwrMax; }

 private:
  Big16 type_ = type();
  Big16 len_ = 40;
  Padding<4> pad_;
  Big<OFPOpticalPortFeaturesFlags> supported_;
  Big32 txMinFreqLmda_;
  Big32 txMaxFreqLmda_;
  Big32 txGridFreqLmda_;
  Big32 rxMinFreqLmda_;
  Big32 rxMaxFreqLmda_;
  Big32 rxGridFreqLmda_;
  Big16 txPwrMin_;
  Big16 txPwrMax_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(PortPropertyOptical) == 40, "Unexpected size.");

using PortPropertyExperimenter =
    detail::ExperimenterProperty<OFPPortProperty, OFPPDPT_EXPERIMENTER>;

}  // namespace ofp

#endif  // OFP_PORTPROPERTY_H_
