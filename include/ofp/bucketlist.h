// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BUCKETLIST_H_
#define OFP_BUCKETLIST_H_

#include "ofp/protocollist.h"
#include "ofp/bucketrange.h"

namespace ofp {

class BucketList : public ProtocolList<BucketRange> {
  using Inherited = ProtocolList<BucketRange>;

 public:
  using Inherited::Inherited;

  void add(const BucketBuilder &bucket);
};

}  // namespace ofp

#endif  // OFP_BUCKETLIST_H_
