// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_INSTRUCTIONTYPE_H_
#define OFP_INSTRUCTIONTYPE_H_

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp {

struct InstructionTypeInfo;

class InstructionType {
 public:
  /* implicit NOLINT */ constexpr InstructionType(
      OFPInstructionType type = OFPIT_GOTO_TABLE)
      : type_{type} {}

  constexpr OFPInstructionType enumType() const { return type_; }

  constexpr operator OFPInstructionType() const { return type_; }

  bool operator==(const InstructionType &rhs) const {
    return type_ == rhs.type_;
  }

  bool operator!=(const InstructionType &rhs) const { return !operator==(rhs); }

  const InstructionTypeInfo *lookupInfo() const;
  bool parse(llvm::StringRef s);
  void setNative(UInt16 value) {
    type_ = static_cast<OFPInstructionType>(value);
  }

 private:
  Big<OFPInstructionType> type_;
};

static_assert(IsLiteralType<InstructionType>(), "Literal type expected.");
static_assert(sizeof(InstructionType) == 2, "Unexpected size.");
static_assert(alignof(InstructionType) == 2, "Unexpected alignment.");

OFP_BEGIN_IGNORE_PADDING

struct InstructionTypeInfo {
  InstructionType type;
  const char *name;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_INSTRUCTIONTYPE_H_
