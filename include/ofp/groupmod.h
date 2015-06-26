// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_GROUPMOD_H_
#define OFP_GROUPMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/bucketlist.h"
#include "ofp/bucketrange.h"
#include "ofp/groupnumber.h"

namespace ofp {

class GroupMod : public ProtocolMsg<GroupMod, OFPT_GROUP_MOD, 16, 65528, true> {
 public:
  OFPGroupModCommand command() const { return command_; }
  OFPGroupType groupType() const { return groupType_; }
  GroupNumber groupId() const { return groupId_; }
  BucketRange buckets() const;

  bool validateInput(Validation *context) const;

 private:
  Header header_;
  Big<OFPGroupModCommand> command_;
  Big<OFPGroupType> groupType_;
  Padding<1> pad_;
  GroupNumber groupId_;

  // Only GroupModBuilder can construct an actual instance.
  GroupMod() : header_{type()} {}

  friend class GroupModBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(GroupMod) == 16, "Unexpected size.");
static_assert(IsStandardLayout<GroupMod>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<GroupMod>(), "Expected trivially copyable.");

class GroupModBuilder {
 public:
  GroupModBuilder() = default;
  explicit GroupModBuilder(const GroupMod *msg);

  void setCommand(OFPGroupModCommand command) { msg_.command_ = command; }
  void setGroupType(OFPGroupType groupType) { msg_.groupType_ = groupType; }
  void setGroupId(UInt32 groupId) { msg_.groupId_ = groupId; }
  void setBuckets(const BucketList &buckets) { buckets_ = buckets; }

  UInt32 send(Writable *channel);

 private:
  GroupMod msg_;
  BucketList buckets_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_GROUPMOD_H_
