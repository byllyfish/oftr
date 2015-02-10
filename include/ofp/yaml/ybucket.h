// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YBUCKET_H_
#define OFP_YAML_YBUCKET_H_

#include "ofp/bucket.h"
#include "ofp/bucketlist.h"
#include "ofp/bucketrange.h"

namespace ofp {
namespace detail {

struct BucketInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::detail::BucketInserter> {
  static void mapping(IO &io, ofp::detail::BucketInserter &inserter) {
    using namespace ofp;

    BucketList &buckets = Ref_cast<BucketList>(inserter);

    UInt16 weight;
    PortNumber watchPort;
    UInt32 watchGroup;
    io.mapRequired("weight", weight);
    io.mapRequired("watch_port", watchPort);
    io.mapRequired("watch_group", watchGroup);

    ActionList actions;
    io.mapRequired("actions", actions);

    BucketBuilder bkt;
    bkt.setWeight(weight);
    bkt.setWatchPort(watchPort);
    bkt.setWatchGroup(watchGroup);
    bkt.setActions(actions);
    buckets.add(bkt);
  }
};

template <>
struct MappingTraits<ofp::Bucket> {
  static void mapping(IO &io, ofp::Bucket &bucket) {
    using namespace ofp;

    Hex16 weight = bucket.weight();
    PortNumber watchPort = bucket.watchPort();
    Hex32 watchGroup = bucket.watchGroup();
    io.mapRequired("weight", weight);
    io.mapRequired("watch_port", watchPort);
    io.mapRequired("watch_group", watchGroup);

    ActionRange actions = bucket.actions();
    io.mapRequired("actions", actions);
  }
};

template <>
struct SequenceTraits<ofp::BucketRange> {
  using iterator = ofp::BucketIterator;

  static iterator begin(IO &io, ofp::BucketRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::BucketRange &range) { return range.end(); }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }
};

template <>
struct SequenceTraits<ofp::BucketList> {
  static size_t size(IO &io, ofp::BucketList &list) { return 0; }

  static ofp::detail::BucketInserter &element(IO &io, ofp::BucketList &list,
                                              size_t index) {
    return Ref_cast<ofp::detail::BucketInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBUCKET_H_
