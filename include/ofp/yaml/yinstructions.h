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

// Pointers are used for read-only.
template <>
struct MappingTraits<ofp::IT_GOTO_TABLE *> {
  static void mapping(IO &io, ofp::IT_GOTO_TABLE *&instr) {
    io.mapRequired("table_id", instr->tableId_);
  }
};

template <>
struct MappingTraits<ofp::IT_GOTO_TABLE> {
  static void mapping(IO &io, ofp::IT_GOTO_TABLE &instr) {
    io.mapRequired("table_id", instr.tableId_);
  }
};

template <>
struct MappingTraits<ofp::IT_WRITE_METADATA *> {
  static void mapping(IO &io, ofp::IT_WRITE_METADATA *&instr) {
    io.mapRequired("metadata", instr->metadata_);
    io.mapRequired("mask", instr->mask_);
  }
};

template <>
struct MappingTraits<ofp::IT_WRITE_METADATA> {
  static void mapping(IO &io, ofp::IT_WRITE_METADATA &instr) {
    io.mapRequired("metadata", instr.metadata_);
    io.mapRequired("mask", instr.mask_);
  }
};

template <>
struct MappingTraits<ofp::IT_METER *> {
  static void mapping(IO &io, ofp::IT_METER *&instr) {
    io.mapRequired("meter", instr->meter_);
  }
};

template <>
struct MappingTraits<ofp::IT_METER> {
  static void mapping(IO &io, ofp::IT_METER &instr) {
    io.mapRequired("meter", instr.meter_);
  }
};

template <>
struct MappingTraits<ofp::IT_EXPERIMENTER *> {
  static void mapping(IO &io, ofp::IT_EXPERIMENTER *&instr) {
    io.mapRequired("meter", instr->experimenterId_);
  }
};

template <>
struct MappingTraits<ofp::IT_EXPERIMENTER> {
  static void mapping(IO &io, ofp::IT_EXPERIMENTER &instr) {
    io.mapRequired("meter", instr.experimenterId_);
  }
};

template <>
struct MappingTraits<ofp::InstructionIterator::Element> {

  static void mapping(IO &io, ofp::InstructionIterator::Element &item) {
    using namespace ofp;

    OFPInstructionType type = item.type();
    io.mapRequired("type", type);

    switch (type) {
    case IT_GOTO_TABLE::type() : {
      IT_GOTO_TABLE *instr =
          RemoveConst_cast(item.instruction<IT_GOTO_TABLE>());
      io.mapRequired("value", instr);
      break;
    }
    case IT_WRITE_METADATA::type() : {
      IT_WRITE_METADATA *instr =
          RemoveConst_cast(item.instruction<IT_WRITE_METADATA>());
      io.mapRequired("value", instr);
      break;
    }
    case IT_WRITE_ACTIONS::type() : {
      IT_WRITE_ACTIONS *instr =
          RemoveConst_cast(item.instruction<IT_WRITE_ACTIONS>());
      ActionRange actions{instr->dataRange()};
      io.mapRequired("value", actions);
      break;
    }
    case IT_APPLY_ACTIONS::type() : {
      IT_APPLY_ACTIONS *instr =
          RemoveConst_cast(item.instruction<IT_APPLY_ACTIONS>());
      ActionRange actions{instr->dataRange()};
      io.mapRequired("value", actions);
      break;
    }
    case IT_CLEAR_ACTIONS::type() : {
      // No value.
      break;
    }
    case IT_METER::type() : {
      IT_METER *instr = RemoveConst_cast(item.instruction<IT_METER>());
      io.mapRequired("value", instr);
      break;
    }
    case IT_EXPERIMENTER::type() : {
      IT_EXPERIMENTER *instr =
          RemoveConst_cast(item.instruction<IT_EXPERIMENTER>());
      io.mapRequired("value", instr);
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
    io.mapRequired("type", type);

    switch (type) {
    case IT_GOTO_TABLE::type() : {
      IT_GOTO_TABLE instr{0};
      io.mapRequired("value", instr);
      list.add(instr);
      break;
    }
    case IT_WRITE_METADATA::type() : {
      IT_WRITE_METADATA instr{0, 0};
      io.mapRequired("value", instr);
      list.add(instr);
      break;
    }
    case IT_WRITE_ACTIONS::type() : {
      ActionList actions;
      io.mapRequired("value", actions);
      list.add(IT_WRITE_ACTIONS{&actions});
      break;
    }
    case IT_APPLY_ACTIONS::type() : {
      ActionList actions;
      io.mapRequired("value", actions);
      list.add(IT_APPLY_ACTIONS{&actions});
      break;
    }
    case IT_CLEAR_ACTIONS::type() : {
      IT_CLEAR_ACTIONS instr;
      // io.mapRequired("value", instr);
      list.add(instr);
      break;
    }
    case IT_METER::type() : {
      IT_METER instr{0};
      io.mapRequired("value", instr);
      list.add(instr);
      break;
    }
    case IT_EXPERIMENTER::type() : {
      IT_EXPERIMENTER instr{0};
      io.mapRequired("value", instr);
      list.add(instr);
      break;
    }
    }
  }
};

template <>
struct SequenceTraits<ofp::InstructionRange> {

  static size_t size(IO &io, ofp::InstructionRange &range) {
    return range.itemCount();
  }

  static ofp::InstructionIterator::Element &
  element(IO &io, ofp::InstructionRange &range, size_t index) {
    // FIXME - iterates every time!
    ofp::InstructionIterator iter = range.begin();
    for (size_t i = 0; i < index; ++i)
      ++iter;
    return RemoveConst_cast(*iter);
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
