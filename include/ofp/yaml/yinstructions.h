// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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

const char *const kGotoTableSchema = R"""({Instruction/GotoTable}
instruction: GOTO_TABLE
table_id: UInt8
)""";

const char *const kWriteMetadataSchema = R"""({Instruction/WriteMetadata}
instruction: WRITE_METADATA
metadata: UInt64
mask: UInt64
)""";

const char *const kWriteActionsSchema = R"""({Instruction/WriteActions}
instruction: WRITE_ACTIONS
actions: [Action]
)""";

const char *const kApplyActionsSchema = R"""({Instruction/ApplyActions}
instruction: APPLY_ACTIONS
actions: [Action]
)""";

const char *const kClearActionsSchema = R"""({Instruction/ClearActions}
instruction: CLEAR_ACTIONS
)""";

const char *const kMeterSchema = R"""({Instruction/Meter}
instruction: METER
meter: UInt32
)""";

const char *const kExperimenterInstructionSchema =
    R"""({Instruction/Experimenter}
instruction: EXPERIMENTER
experimenter: UInt32
data: HexData
)""";

template <>
struct MappingTraits<ofp::InstructionIterator::Element> {
  static void mapping(IO &io, ofp::InstructionIterator::Element &item) {
    using namespace ofp;

    OFPInstructionType type = item.type();
    io.mapRequired("instruction", type);

    switch (type) {
      case IT_GOTO_TABLE::type(): {
        IT_GOTO_TABLE *instr =
            RemoveConst_cast(item.instruction<IT_GOTO_TABLE>());
        Hex8 tableId = instr->tableId();
        io.mapRequired("table_id", tableId);
        break;
      }
      case IT_WRITE_METADATA::type(): {
        IT_WRITE_METADATA *instr =
            RemoveConst_cast(item.instruction<IT_WRITE_METADATA>());
        Hex64 metadata = instr->metadata();
        Hex64 mask = instr->mask();
        io.mapRequired("metadata", metadata);
        io.mapRequired("mask", mask);
        break;
      }
      case IT_WRITE_ACTIONS::type(): {
        IT_WRITE_ACTIONS *instr =
            RemoveConst_cast(item.instruction<IT_WRITE_ACTIONS>());
        ActionRange actions{instr->dataRange()};
        io.mapRequired("actions", actions);
        break;
      }
      case IT_APPLY_ACTIONS::type(): {
        IT_APPLY_ACTIONS *instr =
            RemoveConst_cast(item.instruction<IT_APPLY_ACTIONS>());
        ActionRange actions{instr->dataRange()};
        io.mapRequired("actions", actions);
        break;
      }
      case IT_CLEAR_ACTIONS::type(): {
        // No value.
        break;
      }
      case IT_METER::type(): {
        IT_METER *instr = RemoveConst_cast(item.instruction<IT_METER>());
        Hex32 meter = instr->meter();
        io.mapRequired("meter", meter);
        break;
      }
      case IT_EXPERIMENTER::type(): {
        IT_EXPERIMENTER *instr =
            RemoveConst_cast(item.instruction<IT_EXPERIMENTER>());
        Hex32 experimenterId = instr->experimenterId();
        ByteRange data = instr->dataRange();
        io.mapRequired("experimenter", experimenterId);
        io.mapRequired("data", data);
        break;
      }
      case OFPIT_NONE:
      case OFPIT_STAT_TRIGGER:
      default:
        log::warning("InstructionIterator: Unrecognized instructions");
        break;
    }
  }
};

template <>
struct MappingTraits<ofp::detail::InstructionInserter> {
  static void mapping(IO &io, ofp::detail::InstructionInserter &builder) {
    using namespace ofp;

    InstructionList &list = Ref_cast<InstructionList>(builder);

    OFPInstructionType type = OFPIT_GOTO_TABLE;
    io.mapRequired("instruction", type);

    switch (type) {
      case IT_GOTO_TABLE::type(): {
        UInt8 tableId;
        io.mapRequired("table_id", tableId);
        list.add(IT_GOTO_TABLE{tableId});
        break;
      }
      case IT_WRITE_METADATA::type(): {
        UInt64 metadata, mask;
        io.mapRequired("metadata", metadata);
        io.mapRequired("mask", mask);
        list.add(IT_WRITE_METADATA{metadata, mask});
        break;
      }
      case IT_WRITE_ACTIONS::type(): {
        ActionList actions;
        io.mapRequired("actions", actions);
        list.add(IT_WRITE_ACTIONS{&actions});
        break;
      }
      case IT_APPLY_ACTIONS::type(): {
        ActionList actions;
        io.mapRequired("actions", actions);
        list.add(IT_APPLY_ACTIONS{&actions});
        break;
      }
      case IT_CLEAR_ACTIONS::type(): {
        list.add(IT_CLEAR_ACTIONS{});
        break;
      }
      case IT_METER::type(): {
        UInt32 meter;
        io.mapRequired("meter", meter);
        list.add(IT_METER{meter});
        break;
      }
      case IT_EXPERIMENTER::type(): {
        UInt32 experimenterId;
        ByteList data;
        io.mapRequired("experimenter", experimenterId);
        io.mapRequired("data", data);
        list.add(IT_EXPERIMENTER{experimenterId, data});
        break;
      }
      case OFPIT_NONE:
      case OFPIT_STAT_TRIGGER:
      default:
        log::warning("InstructionInserter: Unrecognized instructions");
        break;
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

  static void next(iterator &iter, iterator iterEnd) { ++iter; }
};

template <>
struct SequenceTraits<ofp::InstructionList> {
  static size_t size(IO &io, ofp::InstructionList &list) { return 0; }

  static ofp::detail::InstructionInserter &element(IO &io,
                                                   ofp::InstructionList &list,
                                                   size_t index) {
    return Ref_cast<ofp::detail::InstructionInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YINSTRUCTIONS_H_
