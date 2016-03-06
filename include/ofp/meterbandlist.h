// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_METERBANDLIST_H_
#define OFP_METERBANDLIST_H_

#include "ofp/meterbandrange.h"
#include "ofp/protocollist.h"

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
