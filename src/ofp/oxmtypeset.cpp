// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmtypeset.h"
#include "ofp/log.h"

using namespace ofp;

bool OXMTypeSet::find(OXMType type) {
  UInt16 oxmClass = type.oxmClass();
  UInt8 oxmField = type.oxmField();

  assert(oxmField < 128);

  for (unsigned i = 0; i < classCount_; ++i) {
    if (classes_[i].oxmClass == oxmClass) {
      return classes_[i].fields.test(oxmField);
    }
  }
  return false;
}

bool OXMTypeSet::add(OXMType type) {
  UInt16 oxmClass = type.oxmClass();
  UInt8 oxmField = type.oxmField();

  assert(oxmField < 128);

  for (unsigned i = 0; i < classCount_; ++i) {
    if (classes_[i].oxmClass == oxmClass) {
      if (!classes_[i].fields.test(oxmField)) {
        classes_[i].fields.set(oxmField);
        return true;
      }
      return false;
    }
  }

  if (classCount_ < MaxOXMClasses) {
    classes_[classCount_].oxmClass = oxmClass;
    classes_[classCount_].fields.set(oxmField);
    ++classCount_;
  } else {
    log_info("Number of OXMTypeSet classes exceeded: ", MaxOXMClasses);
  }

  return true;
}
