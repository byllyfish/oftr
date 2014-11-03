//  ===== ---- ofp/oxmtype.cpp -----------------------------*- C++ -*- =====  //
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
/// \brief Implements OXMType class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/oxmtype.h"
#include "ofp/log.h"
#include "ofp/oxmfields.h"
#include <algorithm>

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
