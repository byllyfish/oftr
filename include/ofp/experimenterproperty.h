// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_EXPERIMENTERPROPERTY_H_
#define OFP_EXPERIMENTERPROPERTY_H_

#include "ofp/byteorder.h"
#include "ofp/byterange.h"

namespace ofp {
namespace detail {

OFP_BEGIN_IGNORE_PADDING

template <class PropEnum, PropEnum PropType>
class ExperimenterProperty {
 public:
  constexpr static PropEnum type() { return PropType; }

  enum { FixedHeaderSize = 12 };

  ExperimenterProperty(UInt32 experimenterId, UInt32 expType,
                       const ByteRange &data)
      : len_(UInt16_narrow_cast(FixedHeaderSize + data.size())),
        experimenter_{experimenterId},
        expType_{expType},
        data_{data} {}

  UInt32 size() const { return len_; }
  UInt32 experimenter() const { return experimenter_; }
  UInt32 expType() const { return expType_; }

  ByteRange expData() const {
    assert(len_ >= FixedHeaderSize);
    return ByteRange{BytePtr(this) + FixedHeaderSize, size() - FixedHeaderSize};
  }

  ByteRange valueRef() const { return data_; }

 private:
  Big16 type_ = type();
  Big16 len_;
  Big32 experimenter_;
  Big32 expType_;
  ByteRange data_;

  friend class PropertyList;
};

OFP_END_IGNORE_PADDING

}  // namespace detail
}  // namespace ofp

#endif  // OFP_EXPERIMENTERPROPERTY_H_
