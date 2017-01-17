// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BUCKETLIST_H_
#define OFP_BUCKETLIST_H_

#include "ofp/bucketrange.h"
#include "ofp/protocollist.h"

namespace ofp {

class BucketList : public ProtocolList<BucketRange> {
  using Inherited = ProtocolList<BucketRange>;

 public:
  using Inherited::Inherited;

  void add(const BucketBuilder &bucket);
};

}  // namespace ofp

#endif  // OFP_BUCKETLIST_H_
