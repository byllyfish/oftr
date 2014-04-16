#ifndef OFP_MPGROUPDESC_H_
#define OFP_MPGROUPDESC_H_

#include "ofp/bucketlist.h"

namespace ofp {

class MPGroupDesc {
 public:
  UInt8 type() const { return type_; }
  UInt32 groupId() const { return groupId_; }
  BucketRange buckets() const;

 private:
  Big16 length_;
  Big8 type_;
  Padding<1> pad_;
  Big32 groupId_;

  friend class MPGroupDescBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(MPGroupDesc) == 8, "Unexpected size.");
static_assert(IsStandardLayout<MPGroupDesc>(), "Expected standard layout.");

class MPGroupDescBuilder {
 public:
  MPGroupDescBuilder() = default;

  void setType(UInt8 type) { msg_.type_ = type; }
  void setGroupId(UInt32 groupId) { msg_.groupId_ = groupId; }
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
