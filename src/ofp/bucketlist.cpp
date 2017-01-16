// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/bucketlist.h"

using namespace ofp;

void BucketList::add(const BucketBuilder &bucket) {
  ActionRange actions = bucket.actions();

  buf_.add(&bucket, BucketBuilder::SizeWithoutActionRange);
  buf_.add(actions.data(), actions.size());
}
