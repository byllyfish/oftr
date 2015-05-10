// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_PORTPROPERTY_H_
#define OFP_PORTPROPERTY_H_

namespace ofp {

class PortPropertyEthernet {
public:
  constexpr static OFPPortProperty type() { return OFPPDPT_ETHERNET; }

  OFPPortFeaturesFlags curr() const { return curr_; }
  OFPPortFeaturesFlags advertised() const { return advertised_; }
  OFPPortFeaturesFlags supported() const { return supported_; }
  OFPPortFeaturesFlags peer() const { return peer_; }
  UInt32 currSpeed() const { return currSpeed_; }
  UInt32 maxSpeed() const { return maxSpeed_; }

  void setCurr(OFPPortFeaturesFlags curr) { curr_ = curr; }
  void setAdvertised(OFPPortFeaturesFlags advertised) { advertised_ = advertised; }
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


class PortPropertyOptical {
public:
  constexpr static OFPPortProperty type() { return OFPPDPT_OPTICAL; }

private:
  Big16 type_ = type();
  Big16 len_ = 40;
  Padding<4> pad_;
  Big32 supported_;
  Big32 txMinFreqLmda_;
  Big32 txMaxFreqLmda_;
  Big32 txGridFreqLmda_;
  Big32 rxMinFreqLmda_;
  Big32 rxMaxFreqLmda_;
  Big32 rxGridFreqLmda_;
  Big16 txPwrMin_;
  Big16 txPwrMax_;
};

}  // namespace ofp

#endif // OFP_PORTPROPERTY_H_
