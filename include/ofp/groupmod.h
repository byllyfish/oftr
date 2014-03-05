//  ===== ---- ofp/groupmod.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the GroupMod and GroupModBuilder class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_GROUPMOD_H_
#define OFP_GROUPMOD_H_

#include "ofp/protocolmsg.h"
#include "ofp/bucketlist.h"
#include "ofp/bucketrange.h"

namespace ofp {

class GroupMod : public ProtocolMsg<GroupMod, OFPT_GROUP_MOD, 16, 65528, true> {
public:
  UInt16 command() const { return command_; }
  UInt8 groupType() const { return groupType_; }
  UInt32 groupId() const { return groupId_; }
  BucketRange buckets() const;

  bool validateInput(size_t length) const;

private:
  Header header_;
  Big16 command_;
  Big8 groupType_;
  Padding<1> pad_;
  Big32 groupId_;

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

  UInt32 send(Writable *channel);

private:
  GroupMod msg_;
  BucketList buckets_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_GROUPMOD_H_
