// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_METERBANDLIST_H_
#define OFP_METERBANDLIST_H_

#include "ofp/protocollist.h"
#include "ofp/meterbandrange.h"

namespace ofp {

class MeterBandList : public ProtocolList<MeterBandRange> {
  using Inherited = ProtocolList<MeterBandRange>;

 public:
  using Inherited::Inherited;

  template <class Type>
  void add(const Type &meterBand);
};

template <class Type>
inline void MeterBandList::add(const Type &meterBand) {
  buf_.add(&meterBand, sizeof(meterBand));
}

}  // namespace ofp

#endif  // OFP_METERBANDLIST_H_
