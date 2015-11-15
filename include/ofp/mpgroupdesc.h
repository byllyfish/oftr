// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_MPGROUPDESC_H_
#define OFP_MPGROUPDESC_H_

#include "ofp/bucketlist.h"
#include "ofp/groupnumber.h"

namespace ofp {

class MPGroupDesc {
 public:
  enum { MPVariableSizeOffset = 0 };

  OFPGroupType type() const { return type_; }
  GroupNumber groupId() const { return groupId_; }
  BucketRange buckets() const;

  bool validateInput(Validation *context) const;

 private:
  Big16 length_;
  Big<OFPGroupType> type_;
  Padding<1> pad_;
  GroupNumber groupId_;

  friend class MPGroupDescBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPGroupDesc) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPGroupDesc>(), "Expected standard layout.");

class MPGroupDescBuilder {
 public:
  MPGroupDescBuilder() = default;

  void setType(OFPGroupType type) { msg_.type_ = type; }
  void setGroupId(GroupNumber groupId) { msg_.groupId_ = groupId; }
  void setBuckets(const BucketRange &buckets);

  void write(Writable *channel);
  void reset() { buckets_.clear(); }

 private:
  MPGroupDesc msg_;
  BucketList buckets_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_MPGROUPDESC_H_
