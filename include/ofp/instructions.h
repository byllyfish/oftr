//  ===== ---- ofp/instructions.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the instruction classes for the OpenFlow protocol.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONS_H_
#define OFP_INSTRUCTIONS_H_

#include "ofp/instructiontype.h"
#include "ofp/padding.h"
#include "ofp/actionlist.h"

namespace ofp {
namespace detail {

struct InstructionHeaderWithPadding {
  InstructionType type_;
  Big16 length_;
  Padding<4> pad_;

  InstructionHeaderWithPadding(InstructionType type, UInt16 length)
      : type_{type}, length_{length} {}
};

static_assert(IsStandardLayout<InstructionHeaderWithPadding>(),
              "Expected standard layout type.");

}  // namespace detail

class IT_GOTO_TABLE {
public:
  constexpr static InstructionType type() {
    return InstructionType{OFPIT_GOTO_TABLE};
  }

  constexpr explicit IT_GOTO_TABLE(UInt8 tableId) : tableId_{tableId} {}

  UInt8 tableId() const { return tableId_; }
  void setTableId(UInt8 tableId) { tableId_ = tableId; }

private:
  InstructionType type_ = type();
  Big16 length_{8};
  Big8 tableId_;
  Padding<3> pad_;

  friend struct llvm::yaml::MappingTraits<IT_GOTO_TABLE>;
  friend struct llvm::yaml::MappingTraits<IT_GOTO_TABLE *>;
};

class IT_WRITE_METADATA {
public:
  constexpr static InstructionType type() {
    return InstructionType{OFPIT_WRITE_METADATA};
  }

  constexpr explicit IT_WRITE_METADATA(UInt64 metadata, UInt64 mask)
      : metadata_{metadata}, mask_{mask} {}

  constexpr UInt64 metadata() const { return metadata_; }
  constexpr UInt64 mask() const { return mask_; }

private:
  InstructionType type_ = type();
  Big16 length_{24};
  Padding<4> pad_;
  Big64 metadata_;
  Big64 mask_;

  friend struct llvm::yaml::MappingTraits<IT_WRITE_METADATA>;
  friend struct llvm::yaml::MappingTraits<IT_WRITE_METADATA *>;
};

namespace detail {

// IT_WithActions is an instruction with a variable sized action list.

template <OFPInstructionType InstrType>
class IT_WithActions {
public:
  enum : bool {
    VariableSize = true
  };
  enum : size_t {
    HeaderSize = 8
  };

  constexpr static InstructionType type() {
    return InstructionType{InstrType};
  }

  explicit IT_WithActions(ActionList *actions)
      : length_{UInt16_narrow_cast(HeaderSize + actions->size())},
        actions_{actions} {}

  const UInt8 *data() const { return actions_->data(); }
  size_t size() const { return actions_->size(); }

  ByteRange dataRange() const {
    assert(length_ >= SizeWithoutActions);
    return ByteRange{BytePtr(this) + SizeWithoutActions,
                     length_ - SizeWithoutActions};
  }

  bool validateInput(const char *context) const {
    if (length_ < SizeWithoutActions)
      return false;
    ActionRange actions = dataRange();
    return actions.validateInput(context);
  }

private:
  InstructionType type_ = type();
  Big16 length_;
  Padding<4> pad_;
  ActionList *actions_; // FIXME - use ActionRange?

  enum : size_t {
    SizeWithoutActions = 8
  };
  // friend struct llvm::yaml::MappingTraits<IT_WithActions<InstrType>>;
  // friend struct llvm::yaml::MappingTraits<IT_WithActions<InstrType>*>;
};

}  // namespace detail

using IT_WRITE_ACTIONS = detail::IT_WithActions<OFPIT_WRITE_ACTIONS>;
using IT_APPLY_ACTIONS = detail::IT_WithActions<OFPIT_APPLY_ACTIONS>;

class IT_CLEAR_ACTIONS {
public:
  constexpr static InstructionType type() {
    return InstructionType{OFPIT_CLEAR_ACTIONS};
  }

  constexpr IT_CLEAR_ACTIONS() {}

private:
  InstructionType type_ = type();
  Big16 length_{8};
  Padding<4> pad_;

  friend struct llvm::yaml::MappingTraits<IT_CLEAR_ACTIONS>;
  friend struct llvm::yaml::MappingTraits<IT_CLEAR_ACTIONS *>;
};

class IT_METER {
public:
  constexpr static InstructionType type() {
    return InstructionType{OFPIT_METER};
  }

  constexpr explicit IT_METER(UInt32 meter) : meter_{meter} {}

  constexpr UInt32 meter() const { return meter_; }

private:
  InstructionType type_ = type();
  Big16 length_{8};
  Big32 meter_;

  friend struct llvm::yaml::MappingTraits<IT_METER>;
  friend struct llvm::yaml::MappingTraits<IT_METER *>;
};

class IT_EXPERIMENTER {
public:
  constexpr static InstructionType type() {
    return InstructionType{OFPIT_EXPERIMENTER};
  }

  constexpr explicit IT_EXPERIMENTER(UInt32 experimenterId)
      : experimenterId_{experimenterId} {}

  constexpr UInt32 experimenterId() const { return experimenterId_; }

private:
  InstructionType type_ = type();
  Big16 length_{8};
  Big32 experimenterId_;

  friend struct llvm::yaml::MappingTraits<IT_EXPERIMENTER>;
  friend struct llvm::yaml::MappingTraits<IT_EXPERIMENTER *>;
};

}  // namespace ofp

#endif  // OFP_INSTRUCTIONS_H_
