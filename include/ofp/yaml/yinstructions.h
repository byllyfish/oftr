//  ===== ---- ofp/yaml/yinstructions.h --------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for all instructions and
/// InstructionIterator::Item and the llvm::yaml::SequenceTraits for
/// InstructionRange and InstructionList.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YINSTRUCTIONS_H_
#define OFP_YAML_YINSTRUCTIONS_H_

#include "ofp/instructionlist.h"
#include "ofp/yaml/ybyteorder.h"
#include "ofp/yaml/yactions.h"
#include "ofp/instructionrange.h"

namespace ofp {
namespace detail {

struct InstructionInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<ofp::InstructionIterator::Element> {

  static void mapping(IO &io, ofp::InstructionIterator::Element &item) {
    using namespace ofp;

    OFPInstructionType type = item.type();
    io.mapRequired("instruction", type);

    switch (type) {
    case IT_GOTO_TABLE::type() : {
      IT_GOTO_TABLE *instr =
          RemoveConst_cast(item.instruction<IT_GOTO_TABLE>());
      UInt8 tableId = instr->tableId();
      io.mapRequired("table_id", tableId);
      break;
    }
    case IT_WRITE_METADATA::type() : {
      IT_WRITE_METADATA *instr =
          RemoveConst_cast(item.instruction<IT_WRITE_METADATA>());
      UInt64 metadata = instr->metadata();
      UInt64 mask = instr->mask();
      io.mapRequired("metadata", metadata);
      io.mapRequired("mask", mask);
      break;
    }
    case IT_WRITE_ACTIONS::type() : {
      IT_WRITE_ACTIONS *instr =
          RemoveConst_cast(item.instruction<IT_WRITE_ACTIONS>());
      ActionRange actions{instr->dataRange()};
      io.mapRequired("actions", actions);
      break;
    }
    case IT_APPLY_ACTIONS::type() : {
      IT_APPLY_ACTIONS *instr =
          RemoveConst_cast(item.instruction<IT_APPLY_ACTIONS>());
      ActionRange actions{instr->dataRange()};
      io.mapRequired("actions", actions);
      break;
    }
    case IT_CLEAR_ACTIONS::type() : {
      // No value.
      break;
    }
    case IT_METER::type() : {
      IT_METER *instr = RemoveConst_cast(item.instruction<IT_METER>());
      UInt32 meter = instr->meter();
      io.mapRequired("meter", meter);
      break;
    }
    case IT_EXPERIMENTER::type() : {
      IT_EXPERIMENTER *instr =
          RemoveConst_cast(item.instruction<IT_EXPERIMENTER>());
      UInt32 experimenterId = instr->experimenterId();
      io.mapRequired("experimenter_id", experimenterId);
      // FIXME - rest of experimenter
      break;
    }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::InstructionInserter> {

  static void mapping(IO &io, ofp::detail::InstructionInserter &builder) {
    using namespace ofp;

    InstructionList &list = reinterpret_cast<InstructionList &>(builder);

    OFPInstructionType type = OFPIT_GOTO_TABLE;
    io.mapRequired("instruction", type);

    switch (type) {
    case IT_GOTO_TABLE::type() : {
      UInt8 tableId;
      io.mapRequired("table_id", tableId);
      list.add(IT_GOTO_TABLE{tableId});
      break;
    }
    case IT_WRITE_METADATA::type() : {
      UInt64 metadata, mask;
      io.mapRequired("metadata", metadata);
      io.mapRequired("mask", mask);
      list.add(IT_WRITE_METADATA{metadata, mask});
      break;
    }
    case IT_WRITE_ACTIONS::type() : {
      ActionList actions;
      io.mapRequired("actions", actions);
      list.add(IT_WRITE_ACTIONS{&actions});
      break;
    }
    case IT_APPLY_ACTIONS::type() : {
      ActionList actions;
      io.mapRequired("actions", actions);
      list.add(IT_APPLY_ACTIONS{&actions});
      break;
    }
    case IT_CLEAR_ACTIONS::type() : {
      list.add(IT_CLEAR_ACTIONS{});
      break;
    }
    case IT_METER::type() : {
      UInt32 meter;
      io.mapRequired("meter", meter);
      list.add(IT_METER{meter});
      break;
    }
    case IT_EXPERIMENTER::type() : {
      UInt32 experimenterId;
      io.mapRequired("experimenter_id", experimenterId);
      list.add(IT_EXPERIMENTER{experimenterId});
      break;
    }
    }
  }
};

template <>
struct SequenceTraits<ofp::InstructionRange> {
  using iterator = ofp::InstructionIterator;

  static iterator begin(IO &io, ofp::InstructionRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::InstructionRange &range) {
    return range.end();
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
  }
};

template <>
struct SequenceTraits<ofp::InstructionList> {

  static size_t size(IO &io, ofp::InstructionList &list) { return 0; }

  static ofp::detail::InstructionInserter &
  element(IO &io, ofp::InstructionList &list, size_t index) {
    return reinterpret_cast<ofp::detail::InstructionInserter &>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YINSTRUCTIONS_H_
