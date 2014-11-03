//  ===== ---- ofp/oxmlist.cpp -----------------------------*- C++ -*- =====  //
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
/// \brief Implements OXMList class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/oxmlist.h"

using namespace ofp;

void OXMList::insert(OXMIterator pos, const void *data, size_t len) {
  buf_.insert(pos.data(), data, len);
}

OXMList::OXMList(const OXMRange &range) {
  buf_.add(range.data(), range.size());
}

OXMIterator OXMList::replace(OXMIterator pos, OXMIterator end, OXMType type,
                             const void *data, size_t len) {
  assert(type.length() == len);
  assert(end.data() > pos.data());

  ptrdiff_t idx = buf_.offset(pos.data());
  size_t newlen = sizeof(OXMType) + len;
  buf_.replaceUninitialized(pos.data(), end.data(), newlen);

  const UInt8 *tptr = reinterpret_cast<const UInt8 *>(&type);
  std::copy(tptr, tptr + sizeof(type), buf_.mutableData() + idx);
  const UInt8 *dptr = static_cast<const UInt8 *>(data);
  std::copy(dptr, dptr + len, buf_.mutableData() + idx + sizeof(type));

  OXMIterator rest{buf_.data() + idx + newlen};
  assert(rest <= this->end());
  assert(this->begin() <= rest);

  return rest;
}

OXMIterator OXMList::findValue(OXMType type) const {
  OXMIterator iterEnd = end();
  for (OXMIterator iter = begin(); iter < iterEnd; ++iter) {
    if (iter->type() == type) return iter;
  }
  return iterEnd;
}

void OXMList::replaceValue(OXMIterator pos, const void *data, size_t len) {
  std::memcpy(const_cast<UInt8 *>(pos->unknownValuePtr()), data, len);
}
