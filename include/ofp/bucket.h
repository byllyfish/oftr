// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_BUCKET_H_
#define OFP_BUCKET_H_

#include "ofp/actionrange.h"
#include "ofp/byteorder.h"
#include "ofp/groupnumber.h"
#include "ofp/portnumber.h"

namespace ofp {

class Bucket : private NonCopyable {
 public:
  enum { ProtocolIteratorSizeOffset = 0, ProtocolIteratorAlignment = 8 };

  UInt16 weight() const { return weight_; }
  PortNumber watchPort() const { return watchPort_; }
  GroupNumber watchGroup() const { return watchGroup_; }
  ActionRange actions() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 len_;
  Big16 weight_ = 0;
  PortNumber watchPort_ = 0;
  GroupNumber watchGroup_ = 0;
  Padding<4> pad_;

  // Only a BucketBuilder can create an instance.
  Bucket() = default;

  friend class BucketBuilder;
};

static_assert(sizeof(Bucket) == 16, "Unexpected size.");
static_assert(IsStandardLayout<Bucket>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Bucket>(), "Expected trivially copyable.");

class BucketBuilder {
 public:
  BucketBuilder() = default;

  void setWeight(UInt16 weight) { bkt_.weight_ = weight; }
  void setWatchPort(PortNumber watchPort) { bkt_.watchPort_ = watchPort; }
  void setWatchGroup(GroupNumber watchGroup) { bkt_.watchGroup_ = watchGroup; }

  ActionRange actions() const { return actions_; }
  void setActions(ActionRange actions);

 private:
  Bucket bkt_;
  ActionRange actions_;

  enum { SizeWithoutActionRange = sizeof(bkt_) };

  friend class BucketList;
};

}  // namespace ofp

#endif  // OFP_BUCKET_H_
