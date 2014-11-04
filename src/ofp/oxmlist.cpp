// Copyright 2014-present Bill Fisher. All rights reserved.

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
