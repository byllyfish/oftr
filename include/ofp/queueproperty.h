#ifndef OFP_QUEUEPROPERTY_H_
#define OFP_QUEUEPROPERTY_H_

#include "ofp/constants.h"

namespace ofp {

class QueuePropertyMinRate {
public:
    enum { Property = OFPQT_MIN_RATE };
    using ValueType = UInt16;

    explicit QueuePropertyMinRate(UInt16 rate) : rate_{rate} {}

    UInt16 value() const { return rate_; }
    static UInt16 defaultValue() { return 0xffff; }

private:
    Big16 property_ = Property;
    Big16 len_ = 16;
    Padding<4> pad_1;
    Big16 rate_;
    Padding<6> pad_2;
};

static_assert(sizeof(QueuePropertyMinRate) == 16, "Unexpected size.");


class QueuePropertyMaxRate {
public:
    enum { Property = OFPQT_MAX_RATE };
    using ValueType = UInt16;

    explicit QueuePropertyMaxRate(UInt16 rate) : rate_{rate} {}

    UInt16 value() const { return rate_; }
    static UInt16 defaultValue() { return 0xffff; }
    
private:
    Big16 property_ = Property;
    Big16 len_ = 16;
    Padding<4> pad_1;
    Big16 rate_;
    Padding<6> pad_2;
};

static_assert(sizeof(QueuePropertyMinRate) == 16, "Unexpected size.");

class QueuePropertyExperimenter {
public:
    enum { Property = OFPQT_EXPERIMENTER };
    enum { FixedSize = 16 };

    QueuePropertyExperimenter(UInt32 experimenterId, const ByteRange &data) : len_(UInt16_narrow_cast(FixedSize + data.size())), experimenter_{experimenterId}, data_{data} {

    }

    ByteRange value() const { return data_; }

private:
    Big16 property_ = Property;
    Big16 len_;
    Padding<4> pad_1;
    Big32 experimenter_;
    Padding<4> pad_2;
    ByteRange data_;
};

static_assert(sizeof(QueuePropertyExperimenter) == QueuePropertyExperimenter::FixedSize + sizeof(ByteRange), "Unexpected size.");

}  // namespace ofp

#endif // OFP_QUEUEPROPERTY_H_
