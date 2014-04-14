#ifndef OFP_METERBANDS_H_
#define OFP_METERBANDS_H_

#include "ofp/padding.h"

namespace ofp {

class MeterBandDrop {
public:
    constexpr static OFPMeterBandType type() { return OFPMBT_DROP; }

    MeterBandDrop(UInt32 rate, UInt32 burstSize) : rate_{rate}, burstSize_{burstSize} {}

    UInt32 rate() const { return rate_; }
    UInt32 burstSize() const { return burstSize_; }

private:
    Big16 type_ = type();
    Big16 len_ = 16;
    Big32 rate_;
    Big32 burstSize_;
    Padding<4> pad_;
};


class MeterBandDscpRemark {
public:
    constexpr static OFPMeterBandType type() { return OFPMBT_DSCP_REMARK; }

    MeterBandDscpRemark(UInt32 rate, UInt32 burstSize, UInt8 precLevel) : rate_{rate}, burstSize_{burstSize}, precLevel_{precLevel} {}

    UInt32 rate() const { return rate_; }
    UInt32 burstSize() const { return burstSize_; }
    UInt8 precLevel() const { return precLevel_; }

private:
    Big16 type_ = type();
    Big16 len_ = 16;
    Big32 rate_;
    Big32 burstSize_;
    Big8 precLevel_;
    Padding<3> pad_;
};

}  // namespace ofp

#endif // OFP_METERBANDS_H_
