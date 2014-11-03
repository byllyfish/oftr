#include "ofp/instructiontype.h"
#include "ofp/instructions.h"

using namespace ofp;

static const InstructionTypeInfo sInstructionInfo[] = {
    {IT_GOTO_TABLE::type(), "OFPIT_GOTO_TABLE"},
    {IT_WRITE_METADATA::type(), "OFPIT_WRITE_METADATA"},
    {IT_WRITE_ACTIONS::type(), "OFPIT_WRITE_ACTIONS"},
    {IT_APPLY_ACTIONS::type(), "OFPIT_APPLY_ACTIONS"},
    {IT_CLEAR_ACTIONS::type(), "OFPIT_CLEAR_ACTIONS"},
    {IT_METER::type(), "OFPIT_METER"},
    {IT_EXPERIMENTER::type(), "OFPIT_EXPERIMENTER"},
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
