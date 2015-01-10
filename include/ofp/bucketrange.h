// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_BUCKETRANGE_H_
#define OFP_BUCKETRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/bucket.h"

namespace ofp {

using BucketIterator = ProtocolIterator<Bucket, ProtocolIteratorType::Bucket>;
using BucketRange = ProtocolRange<BucketIterator>;

}  // namespace ofp

#endif  // OFP_BUCKETRANGE_H_
