//  ===== ---- ofp/transmogrify.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the Transmogrify class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_TRANSMOGRIFY_H_
#define OFP_TRANSMOGRIFY_H_

#include "ofp/actioniterator.h"
#include "ofp/instructionrange.h"
#include "ofp/actionrange.h"
#include "ofp/bytelist.h"
#include "ofp/oxmlist.h"
#include "ofp/log.h"

namespace ofp {

class Message;
class Header;

class Transmogrify {
public:
  Transmogrify(Message *message);

  void normalize();
  void normalizeFeaturesReplyV1();
  void normalizeFlowModV1();
  void normalizePortStatusV1();
  void normalizeExperimenterV1();
  void normalizePacketOutV1();
  void normalizePortModV1();
  void normalizeFlowRemovedV1();
  void normalizeMultipartRequestV1();
  void normalizeMultipartReplyV1();
  void normalizeMultipartReplyV4();
  
  void normalizeMPFlowRequestV1();
  void normalizeMPFlowReplyV1(size_t *start);
  void normalizeMPTableStatsReplyV4(size_t *start);
  void normalizeMPPortOrQueueStatsReplyV1(size_t *start, size_t len);
  void normalizeMPPortStatsRequestV1();

  UInt32 normPortNumberV1(const UInt8 *ptr);

  int normInstructionsV1orV2(const InstructionRange &instr, UInt8 ipProto);
  int normActionsV1orV2(const ActionRange &actions, UInt8 ipProto);
  int normActionV1orV2(UInt16 type, ActionIterator *iter,
                       ActionIterator *iterEnd, UInt8 ipProto);

  template <class Type>
  int normSetField(ActionIterator *iter, ActionIterator *iterEnd);

  int normOutput(ActionIterator *iter, ActionIterator *iterEnd);

private:
  ByteList &buf_;

  Header *header() { return reinterpret_cast<Header *>(buf_.mutableData()); }
};

template <class Type>
int Transmogrify::normSetField(ActionIterator *iter, ActionIterator *iterEnd) {
  size_t valueLen = iter->valueSize();

  if (valueLen >= sizeof(Type)) {
    typename Type::NativeType value;
    memcpy(&value, iter->valuePtr(), sizeof(value));

    OXMList list;
    list.add(Type{value});
    list.pad8(4);

    int lengthChange =
        static_cast<int>(Signed_cast(list.size()) - Signed_cast(valueLen));
    if (lengthChange > 0) {
      ptrdiff_t offset = buf_.offset(iter->data());
      buf_.insertUninitialized(iter->valuePtr(), Unsigned_cast(lengthChange));
      *iter = ActionIterator{buf_.data() + offset};
      *iterEnd = ActionIterator{buf_.end()};
    } else if (lengthChange < 0) {
      ptrdiff_t offset = buf_.offset(iter->data());
      buf_.remove(iter->data(), Unsigned_cast(-lengthChange));
      *iter = ActionIterator{buf_.data() + offset};
      *iterEnd = ActionIterator{buf_.end()};
    }

    ActionType setField{OFPAT_SET_FIELD, UInt16_narrow_cast(4 + list.size())};
    std::memcpy(RemoveConst_cast(iter->data()), &setField, sizeof(ActionType));
    std::memcpy(RemoveConst_cast(iter->valuePtr()), list.data(), list.size());

    return lengthChange;

  } else {
    log::info("Transmogrify::normSetField: Unexpected value size.", valueLen);
    log::info(" actiontype:", iter->type());
  }

  return 0;
}

}  // namespace ofp

#endif  // OFP_TRANSMOGRIFY_H_
