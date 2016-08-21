// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actionfulltype.h"
#include "ofp/nicira.h"

using namespace ofp;

static const ActionTypeInfo sActionFullInfo[] = {
    {nx::AT_REGMOVE::type(), "NX_REG_MOVE", nx::AT_REGMOVE::experimenter(),
     nx::AT_REGMOVE::subtype()},
    {nx::AT_REGLOAD::type(), "NX_REG_LOAD", nx::AT_REGLOAD::experimenter(),
     nx::AT_REGLOAD::subtype()},
};

bool ActionFullType::parse(const std::string &s) {
  // Try to parse action as a standard action.
  if (type_.parse(s)) {
    experimenter_ = 0;
    subtype_ = 0;
    return true;
  }

  // Try to parse action as an experimenter action.
  for (unsigned i = 0; i < ArrayLength(sActionFullInfo); ++i) {
    if (s == sActionFullInfo[i].name) {
      type_ = sActionFullInfo[i].type;
      experimenter_ = sActionFullInfo[i].experimenter;
      subtype_ = sActionFullInfo[i].subtype;
      return true;
    }
  }

  return false;
}

const ActionTypeInfo *ActionFullType::lookupInfo() const {
  if (experimenter_ == 0)
    return type_.lookupInfo();

  assert(type_.enumType() == OFPAT_EXPERIMENTER);

  for (unsigned i = 0; i < ArrayLength(sActionFullInfo); ++i) {
    if (type_ == sActionFullInfo[i].type &&
        subtype_ == sActionFullInfo[i].subtype) {
      return &sActionFullInfo[i];
    }
  }

  return nullptr;
}

void ActionFullType::setNative(UInt16 enumType, UInt32 experimenter) {
  type_ = ActionType{static_cast<OFPActionType>(enumType), 0};
  experimenter_ = experimenter;
}
