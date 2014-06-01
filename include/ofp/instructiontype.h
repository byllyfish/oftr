//  ===== ---- ofp/instructiontype.h -----------------------*- C++ -*- =====  //
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
/// \brief Defines the InstructionType class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONTYPE_H_
#define OFP_INSTRUCTIONTYPE_H_

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp {

struct InstructionTypeInfo;

class InstructionType {
public:
  constexpr InstructionType(OFPInstructionType type = OFPIT_GOTO_TABLE) : type_{type} {}

  constexpr OFPInstructionType enumType() const { return type_; }

  constexpr operator OFPInstructionType() const { return type_; }

  bool operator==(const InstructionType &rhs) const {
    return type_ == rhs.type_;
  }

  bool operator!=(const InstructionType &rhs) const { return !operator==(rhs); }

  const InstructionTypeInfo *lookupInfo() const;
  bool parse(const std::string &s);

private:
  Big<OFPInstructionType> type_;
};

static_assert(IsLiteralType<InstructionType>(), "Literal type expected.");

std::ostream &operator<<(std::ostream &os, const InstructionType &value);

inline std::ostream &operator<<(std::ostream &os,
                                const InstructionType &value) {
  return os << static_cast<unsigned>(value.enumType());
}


OFP_BEGIN_IGNORE_PADDING

struct InstructionTypeInfo {
  InstructionType type;
  const char *name;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_INSTRUCTIONTYPE_H_
