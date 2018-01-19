// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_METERBANDRANGE_H_
#define OFP_METERBANDRANGE_H_

#include "ofp/constants.h"
#include "ofp/protocolrange.h"

namespace ofp {

namespace detail {

class MeterBandIteratorItem : private NonCopyable {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(Big16),
    ProtocolIteratorAlignment = 8
  };

  OFPMeterBandType type() const { return type_; }
  UInt16 size() const { return len_; }

  template <class Type>
  const Type &meterBand() const {
    return *Interpret_cast<Type>(this);
  }

  bool validateInput(Validation *context) const { return true; }

 private:
  Big<OFPMeterBandType> type_;
  Big16 len_;
};

}  // namespace detail

using MeterBandIterator = ProtocolIterator<detail::MeterBandIteratorItem,
                                           ProtocolIteratorType::MeterBand>;
using MeterBandRange = ProtocolRange<MeterBandIterator>;

}  // namespace ofp

#endif  // OFP_METERBANDRANGE_H_
