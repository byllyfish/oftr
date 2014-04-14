#ifndef OFP_METERBANDRANGE_H_
#define OFP_METERBANDRANGE_H_

#include "ofp/protocolrange.h"

namespace ofp {

namespace detail {

class MeterBandIteratorItem : private NonCopyable {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(Big16)
  };

  OFPMeterBandType type() const { return type_; }
  UInt16 size() const { return len_; }

  template <class Type>
  const Type &meterBand() const {
    return *reinterpret_cast<const Type *>(this);
  }

 private:
  Big<OFPMeterBandType> type_;
  Big16 len_;
};

}  // namespace detail

using MeterBandIterator = ProtocolIterator<detail::MeterBandIteratorItem>;
using MeterBandRange = ProtocolRange<MeterBandIterator>;

}  // namespace ofp

#endif // OFP_METERBANDRANGE_H_
