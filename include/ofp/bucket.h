//  ===== ---- ofp/bucket.h --------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Bucket class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_BUCKET_H_
#define OFP_BUCKET_H_

#include "ofp/byteorder.h"
#include "ofp/actionrange.h"

namespace ofp {

class Bucket : private NonCopyable {
 public:
  enum { ProtocolIteratorSizeOffset = 0, ProtocolIteratorAlignment = 8 };

  UInt16 weight() const { return weight_; }
  UInt32 watchPort() const { return watchPort_; }
  UInt32 watchGroup() const { return watchGroup_; }
  ActionRange actions() const;

 private:
  Big16 len_;
  Big16 weight_ = 0;
  Big32 watchPort_ = 0;
  Big32 watchGroup_ = 0;
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
  void setWatchPort(UInt32 watchPort) { bkt_.watchPort_ = watchPort; }
  void setWatchGroup(UInt32 watchGroup) { bkt_.watchGroup_ = watchGroup; }

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
