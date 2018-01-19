// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
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

bool ActionFullType::parse(llvm::StringRef s) {
  // Try to parse action as a standard action.
  if (type_.parse(s)) {
    experimenter_ = 0;
    subtype_ = 0;
    return true;
  }

  // Try to parse action as an experimenter action.
  for (const auto &i : sActionFullInfo) {
    if (s == i.name) {
      type_ = i.type;
      experimenter_ = i.experimenter;
      subtype_ = i.subtype;
      return true;
    }
  }

  return false;
}

const ActionTypeInfo *ActionFullType::lookupInfo() const {
  if (experimenter_ == 0) {
    return type_.lookupInfo();
  }

  assert(type_.enumType() == OFPAT_EXPERIMENTER);

  for (const auto &i : sActionFullInfo) {
    if (type_ == i.type && subtype_ == i.subtype) {
      return &i;
    }
  }

  return nullptr;
}

void ActionFullType::setNative(UInt16 enumType, UInt32 experimenter) {
  type_ = ActionType{static_cast<OFPActionType>(enumType), 0};
  experimenter_ = experimenter;
}
