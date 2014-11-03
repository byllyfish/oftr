//  ===== ---- ofp/setasync.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the SetAsync and SetAsyncBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SETASYNC_H_
#define OFP_SETASYNC_H_

#include "ofp/protocolmsg.h"

namespace ofp {

class SetAsync : public ProtocolMsg<SetAsync, OFPT_SET_ASYNC, 32, 32> {
 public:
  UInt32 masterPacketInMask() const;
  UInt32 slavePacketInMask() const;
  UInt32 masterPortStatusMask() const;
  UInt32 slavePortStatusMask() const;
  UInt32 masterFlowRemovedMask() const;
  UInt32 slaveFlowRemovedMask() const;

  bool validateInput(Validation *context) const { return true; }

 private:
  Header header_;
  Big32 packetInMask_[2];
  Big32 portStatusMask_[2];
  Big32 flowRemovedMask_[2];

  // Only SetAsyncBuilder can construct an instance.
  SetAsync() : header_{type()} {}

  friend class SetAsyncBuilder;
  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

static_assert(sizeof(SetAsync) == 32, "Unexpected size.");
static_assert(IsStandardLayout<SetAsync>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<SetAsync>(), "Expected trivially copyable.");

class SetAsyncBuilder {
 public:
  SetAsyncBuilder() = default;
  explicit SetAsyncBuilder(const SetAsync *msg);

  void setMasterPacketInMask(UInt32 mask);
  void setSlavePacketInMask(UInt32 mask);
  void setMasterPortStatusMask(UInt32 mask);
  void setSlavePortStatusMask(UInt32 mask);
  void setMasterFlowRemovedMask(UInt32 mask);
  void setSlaveFlowRemovedMask(UInt32 mask);

  UInt32 send(Writable *channel);

 private:
  SetAsync msg_;

  template <class T>
  friend struct llvm::yaml::MappingTraits;
};

}  // namespace ofp

#endif  // OFP_SETASYNC_H_
