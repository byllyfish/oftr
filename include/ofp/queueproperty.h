#ifndef OFP_QUEUEPROPERTY_H_
#define OFP_QUEUEPROPERTY_H_

#include "ofp/constants.h"

namespace ofp {

class QueuePropertyMinRate {
 public:
  constexpr static OFPQueueProperty type() { return OFPQT_MIN_RATE; }

  using ValueType = UInt16;

  explicit QueuePropertyMinRate(UInt16 rate) : rate_{rate} {}

  UInt16 value() const { return rate_; }
  static UInt16 defaultValue() { return 0xffff; }

 private:
  Big16 type_ = type();
  Big16 len_ = 16;
  Padding<4> pad_1;
  Big16 rate_;
  Padding<6> pad_2;
};

static_assert(sizeof(QueuePropertyMinRate) == 16, "Unexpected size.");

class QueuePropertyMaxRate {
 public:
  constexpr static OFPQueueProperty type() { return OFPQT_MAX_RATE; }

  using ValueType = UInt16;

  explicit QueuePropertyMaxRate(UInt16 rate) : rate_{rate} {}

  UInt16 value() const { return rate_; }
  static UInt16 defaultValue() { return 0xffff; }

 private:
  Big16 type_ = type();
  Big16 len_ = 16;
  Padding<4> pad_1;
  Big16 rate_;
  Padding<6> pad_2;
};

static_assert(sizeof(QueuePropertyMinRate) == 16, "Unexpected size.");

class QueuePropertyExperimenter {
 public:
  constexpr static OFPQueueProperty type() { return OFPQT_EXPERIMENTER; }

  enum { FixedHeaderSize = 16 };

  QueuePropertyExperimenter(UInt32 experimenterId, const ByteRange &data)
      : len_(UInt16_narrow_cast(FixedHeaderSize + data.size())),
        experimenter_{experimenterId},
        data_{data} {}

  UInt32 size() const { return len_; }
  UInt32 experimenter() const { return experimenter_; }
  ByteRange value() const {
    return ByteRange{BytePtr(this) + FixedHeaderSize, size() - FixedHeaderSize};
  }
  ByteRange valueRef() const { return data_; }

 private:
  Big16 type_ = type();
  Big16 len_;
  Padding<4> pad_1;
  Big32 experimenter_;
  Padding<4> pad_2;
  ByteRange data_;

  friend class PropertyList;
};

static_assert(sizeof(QueuePropertyExperimenter) ==
                  QueuePropertyExperimenter::FixedHeaderSize +
                      sizeof(ByteRange),
              "Unexpected size.");

class QueuePropertyValidator {
 public:
  static bool validateInput(PropertyRange range);
};

}  // namespace ofp

#endif  // OFP_QUEUEPROPERTY_H_
