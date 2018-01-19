// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/bucket.h"

using namespace ofp;

ActionRange Bucket::actions() const {
  return SafeByteRange(this, len_, sizeof(Bucket));
}

bool Bucket::validateInput(Validation *context) const {
  if (len_ < sizeof(Bucket))
    return false;

  return actions().validateInput(context);
}

void BucketBuilder::setActions(ActionRange actions) {
  bkt_.len_ = UInt16_narrow_cast(sizeof(Bucket) + actions.size());
  actions_ = actions;
}
