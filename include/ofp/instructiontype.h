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

#ifndef OFP_INSTRUCTIONTYPE_H
#define OFP_INSTRUCTIONTYPE_H

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class InstructionType {
public:
  constexpr InstructionType(OFPInstructionType type) : type_{type} {}

  static InstructionType fromBytes(const UInt8 *data);

  constexpr OFPInstructionType type() const { return type_; }

  constexpr operator OFPInstructionType() const { return type_; }

  bool operator==(const InstructionType &rhs) const {
    return type_ == rhs.type_;
  }

  bool operator!=(const InstructionType &rhs) const { return !operator==(rhs); }

private:
  const Big<OFPInstructionType> type_;
};

static_assert(IsLiteralType<InstructionType>(), "Literal type expected.");

std::ostream &operator<<(std::ostream &os, const InstructionType &value);

inline InstructionType InstructionType::fromBytes(const UInt8 *data) {
  InstructionType type{OFPIT_GOTO_TABLE};
  std::memcpy(&type, data, sizeof(type));
  return type;
}

inline std::ostream &operator<<(std::ostream &os,
                                const InstructionType &value) {
  return os << static_cast<unsigned>(value.type());
}

} // </namespace ofp>

#endif // OFP_INSTRUCTIONTYPE_H
