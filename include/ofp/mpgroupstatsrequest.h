// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MPGROUPSTATSREQUEST_H_
#define OFP_MPGROUPSTATSREQUEST_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"
#include "ofp/groupnumber.h"

namespace ofp {

class Writable;
class MultipartRequest;
class Validation;

class MPGroupStatsRequest {
 public:
  static const MPGroupStatsRequest *cast(const MultipartRequest *req);

  GroupNumber groupId() const { return groupId_; }

  bool validateInput(Validation *context) const;

 private:
  GroupNumber groupId_;
  Padding<4> pad_;

  friend class MPGroupStatsRequestBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPGroupStatsRequest) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPGroupStatsRequest>(),
              "Expected standard layout.");

class MPGroupStatsRequestBuilder {
 public:
  MPGroupStatsRequestBuilder() = default;

  void setGroupId(GroupNumber groupId) { msg_.groupId_ = groupId; }

  void write(Writable *channel);
  void reset() {}

 private:
  MPGroupStatsRequest msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPGROUPSTATSREQUEST_H_
