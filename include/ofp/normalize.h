// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_NORMALIZE_H_
#define OFP_NORMALIZE_H_

#include "ofp/actioniterator.h"
#include "ofp/actionrange.h"
#include "ofp/bytelist.h"
#include "ofp/instructionrange.h"
#include "ofp/log.h"
#include "ofp/oxmlist.h"

namespace ofp {

class Message;
class Header;

class Normalize {
 public:
  explicit Normalize(Message *message);

  void normalize();
  void normalizeFeaturesReplyV1();
  void normalizeFeaturesReplyV2();
  void normalizeFlowModV1();
  void normalizePortStatusV1();
  void normalizePortStatusV2();
  void normalizeExperimenterV1();
  void normalizePacketOutV1();
  void normalizePortModV1();
  void normalizePortModV2();
  void normalizeFlowRemovedV1();
  void normalizeMultipartRequestV1();
  void normalizeMultipartReplyV1();
  void normalizeMultipartReplyV2();
  void normalizeMultipartReplyV3();
  void normalizeMultipartReplyV4();
  void normalizeMultipartReplyV5();
  void normalizeAsyncConfigV4();
  void normalizeQueueGetConfigReplyV1();
  void normalizeQueueGetConfigReplyV2();

  void normalizeMPFlowRequestV1();
  void normalizeMPFlowReplyV1(size_t *start);
  void normalizeMPTableStatsReplyV4(size_t *start);
  void normalizeMPPortOrQueueStatsReplyV1(size_t *start, size_t len);
  void normalizeMPPortStatsRequestV1();
  void normalizeMPPortOrQueueStatsReplyV3(size_t *start, size_t len);
  void normalizeMPGroupStatsReplyV3(size_t *start, size_t len);
  void normalizeMPPortDescReplyV1();
  void normalizeMPPortDescReplyV4();
  void normalizeMPPortStatsReplyV1(size_t *start);
  void normalizeMPPortStatsReplyV2(size_t *start);
  void normalizeMPPortStatsReplyV4(size_t *start);

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

  void markInputTooBig(const char *msg);
  void markInputInvalid(const char *msg);
};

template <class Type>
int Normalize::normSetField(ActionIterator *iter, ActionIterator *iterEnd) {
  // Size must be multiple of 8.
  if (((*iter)->size() % 8) != 0) {
    return 0;
  }

  ByteRange valueRange = (*iter)->value();
  size_t valueLen = valueRange.size();

  if (valueLen >= sizeof(Type)) {
    typename Type::NativeType value;
    memcpy(&value, valueRange.data(), sizeof(value));

    OXMList list;
    list.add(Type{value});
    list.pad8(4);

    int lengthChange =
        static_cast<int>(Signed_cast(list.size()) - Signed_cast(valueLen));

    ptrdiff_t offset = buf_.offset(iter->data());
    ptrdiff_t endOffset = buf_.offset(iterEnd->data());

    if (lengthChange > 0) {
      buf_.insertUninitialized(valueRange.data(), Unsigned_cast(lengthChange));

    } else if (lengthChange < 0) {
      buf_.remove(iter->data(), Unsigned_cast(-lengthChange));
    }

    ActionRange range{
        {buf_.data() + offset, buf_.data() + endOffset + lengthChange}};
    *iter = range.begin();
    *iterEnd = range.end();

    valueRange = (*iter)->value();

    ActionType setField{OFPAT_SET_FIELD, UInt16_narrow_cast(4 + list.size())};
    std::memcpy(RemoveConst_cast(iter->data()), &setField, sizeof(ActionType));
    std::memcpy(RemoveConst_cast(valueRange.data()), list.data(), list.size());

    return lengthChange;

  } else {
    log_info("Normalize::normSetField: Unexpected value size.", valueLen);
    log_info(" actiontype:", (*iter)->type());
  }

  return 0;
}

}  // namespace ofp

#endif  // OFP_NORMALIZE_H_
