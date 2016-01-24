// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_EXPERIMENTERPROPERTY_H_
#define OFP_EXPERIMENTERPROPERTY_H_

#include "ofp/byteorder.h"
#include "ofp/byterange.h"
#include "ofp/validation.h"

namespace ofp {

class PropertyList;
class Validation;

namespace detail {

OFP_BEGIN_IGNORE_PADDING

template <class PropEnum, PropEnum PropType>
class ExperimenterProperty : private NonCopyable {
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

  bool validateInput(Validation *context) const {
    return context->validateBool(len_ >= FixedHeaderSize,
                                 "Invalid Experimenter property");
  }

 private:
  Big16 type_ = type();
  Big16 len_;
  Big32 experimenter_;
  Big32 expType_;
  ByteRange data_;

  friend class ofp::PropertyList;
};

OFP_END_IGNORE_PADDING

}  // namespace detail
}  // namespace ofp

#endif  // OFP_EXPERIMENTERPROPERTY_H_
