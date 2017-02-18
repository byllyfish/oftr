// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmlist.h"

using namespace ofp;

void OXMList::insert(OXMIterator pos, const void *data, size_t len) {
  buf_.insert(pos.data(), data, len);
}

OXMList::OXMList(const OXMRange &range) {
  buf_.add(range.data(), range.size());
}

void OXMList::addOrdered(OXMType type, const void *data, size_t len) {
  auto pos = findOrderedPos(type);
  auto idx = buf_.offset(pos.data());
  buf_.insertUninitialized(pos.data(), sizeof(type) + len);
  std::memcpy(buf_.mutableData() + idx, &type, sizeof(type));
  std::memcpy(buf_.mutableData() + idx + sizeof(type), data, len);
}

OXMIterator OXMList::replace(OXMIterator pos, OXMIterator end, OXMType type,
                             const void *data, size_t len) {
  assert(type.length() == len);
  assert(end.data() > pos.data());

  const ptrdiff_t idx = buf_.offset(pos.data());
  const size_t newlen = sizeof(type) + len;
  buf_.replaceUninitialized(pos.data(), end.data(), newlen);
  std::memcpy(buf_.mutableData() + idx, &type, sizeof(type));
  std::memcpy(buf_.mutableData() + idx + sizeof(type), data, len);

  OXMIterator rest{buf_.data() + idx + newlen};
  assert(rest <= this->end());
  assert(this->begin() <= rest);

  return rest;
}

OXMIterator OXMList::findValue(OXMType type) const {
  OXMIterator iterEnd = end();
  for (OXMIterator iter = begin(); iter < iterEnd; ++iter) {
    if (iter->type() == type)
      return iter;
  }
  return iterEnd;
}

OXMIterator OXMList::findOrderedPos(OXMType type) const {
  OXMIterator iterEnd = end();
  auto nativeType = type.oxmNative();
  for (OXMIterator iter = begin(); iter < iterEnd; ++iter) {
    if (iter->type().oxmNative() > nativeType)
      return iter;
  }
  return iterEnd;
}

void OXMList::replaceValue(OXMIterator pos, const void *data, size_t len) {
  std::memcpy(RemoveConst_cast(pos->unknownValuePtr()), data, len);
}
