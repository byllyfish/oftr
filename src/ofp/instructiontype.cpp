// Copyright 2014-present Bill Fisher. All rights reserved.

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
  for (unsigned i = 0; i < ArrayLength(sInstructionInfo); ++i) {
    if (s == sInstructionInfo[i].name) {
      type_ = sInstructionInfo[i].type.type_;
      return true;
    }
  }
  return false;
}

const InstructionTypeInfo *InstructionType::lookupInfo() const {
  InstructionType desiredValue = *this;
  for (unsigned i = 0; i < ArrayLength(sInstructionInfo); ++i) {
    if (desiredValue == sInstructionInfo[i].type) {
      return &sInstructionInfo[i];
    }
  }

  return nullptr;
}
