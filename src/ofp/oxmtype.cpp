// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmtype.h"
#include <algorithm>
#include "ofp/log.h"
#include "ofp/oxmfields.h"

using namespace ofp;

const OXMTypeInfo *OXMType::lookupInfo() const {
  unsigned idx = static_cast<unsigned>(internalID());
  if (idx < OXMTypeInfoArraySize) {
    return &OXMTypeInfoArray[idx];
  }

  return nullptr;
}

const OXMTypeInfo *OXMType::lookupInfo_IgnoreLength() const {
  unsigned idx = static_cast<unsigned>(internalID_IgnoreLength());
  if (idx < OXMTypeInfoArraySize) {
    return &OXMTypeInfoArray[idx];
  }

  return nullptr;
}

// \returns Internal ID for OXMType, or OXMInternalID::UNKNOWN if not found.
OXMInternalID OXMType::internalID() const {
  // Get unmasked value before we search for it.
  UInt32 value32 = hasMask() ? withoutMask() : value32_;

  const OXMTypeInternalMapEntry *begin = &OXMTypeInternalMapArray[0];
  const OXMTypeInternalMapEntry *end = begin + OXMTypeInfoArraySize;

  begin = std::lower_bound(begin, end, value32,
                           [](const OXMTypeInternalMapEntry &item,
                              UInt32 value) { return item.value32 < value; });

  if (begin != end && begin->value32 == value32) {
    return begin->id;
  }

  return OXMInternalID::UNKNOWN;
}

// \returns Internal ID for OXMType, or OXMInternalID::UNKNOWN if not found.
OXMInternalID OXMType::internalID_IgnoreLength() const {
  // Get unmasked value before we search for it.
  UInt32 value32 = (withoutMask() & Prefix24Bits);

  const OXMTypeInternalMapEntry *begin = &OXMTypeInternalMapArray[0];
  const OXMTypeInternalMapEntry *end = begin + OXMTypeInfoArraySize;

  begin =
      std::find_if(begin, end, [value32](const OXMTypeInternalMapEntry &item) {
        return (item.value32 & Prefix24Bits) == value32;
      });
  if (begin != end) {
    return begin->id;
  }

  return OXMInternalID::UNKNOWN;
}

// \returns Internal ID for OXMType, or OXMInternalID::UNKNOWN if not found.
OXMInternalID OXMType::internalID_Experimenter(Big32 experimenter) const {
  // Get unmasked value before we search for it.
  UInt32 value32 = hasMask() ? withoutMask() : value32_;

  const OXMTypeInternalMapEntry *begin = &OXMTypeInternalMapArray[0];
  const OXMTypeInternalMapEntry *end = begin + OXMTypeInfoArraySize;

  begin = std::lower_bound(begin, end, value32,
                           [](const OXMTypeInternalMapEntry &item,
                              UInt32 value) { return item.value32 < value; });

  // Linear search for experimenter. (TODO: Sort by experimenter also...)
  while (begin != end && begin->value32 == value32) {
    if (begin->experimenter == experimenter)
      return begin->id;
    ++begin;
  }

  return OXMInternalID::UNKNOWN;
}

bool OXMType::parse(const std::string &s) {
  // TODO(bfish): make faster
  for (size_t i = 0; i < OXMTypeInfoArraySize; ++i) {
    if (s == OXMTypeInfoArray[i].name) {
      value32_ = OXMTypeInfoArray[i].value32;
      return true;
    }
  }

  return false;
}

std::string OXMType::toString() const {
  auto info = lookupInfo();
  if (info) {
    return info->name;
  }
  return RawDataToHex(&value32_, sizeof(value32_));
}
