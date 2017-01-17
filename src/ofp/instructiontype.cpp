// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/instructiontype.h"
#include "ofp/instructions.h"

using namespace ofp;

static const InstructionTypeInfo sInstructionInfo[] = {
    {IT_GOTO_TABLE::type(), "GOTO_TABLE"},
    {IT_WRITE_METADATA::type(), "WRITE_METADATA"},
    {IT_WRITE_ACTIONS::type(), "WRITE_ACTIONS"},
    {IT_APPLY_ACTIONS::type(), "APPLY_ACTIONS"},
    {IT_CLEAR_ACTIONS::type(), "CLEAR_ACTIONS"},
    {IT_METER::type(), "METER"},
    {IT_EXPERIMENTER::type(), "EXPERIMENTER"},
};

bool InstructionType::parse(const std::string &s) {
  for (const auto &i : sInstructionInfo) {
    if (s == i.name) {
      type_ = i.type.type_;
      return true;
    }
  }
  return false;
}

const InstructionTypeInfo *InstructionType::lookupInfo() const {
  InstructionType desiredValue = *this;
  for (const auto &i : sInstructionInfo) {
    if (desiredValue == i.type) {
      return &i;
    }
  }

  return nullptr;
}
