// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "./ofpx_util.h"
#include <vector>
#include "ofp/constants.h"
#include "ofp/byteorder.h"
#include "ofp/padding.h"

using namespace ofp;

static bool normalizeTableFeaturesEntry(const UInt8 *&ptr,
                                        size_t &remainingLength,
                                        ByteList &results);

struct Prop {
  Big16 type;
  Big16 length;
};

struct EmptyProp {
  Big16 type;
  Big16 length;
  Big32 empty;
};

static const EmptyProp kDefaultEmptyProperties[] = {
    {OFPTFPT_INSTRUCTIONS, 4, 0},
    {OFPTFPT_NEXT_TABLES, 4, 0},
    {OFPTFPT_WRITE_ACTIONS, 4, 0},
    {OFPTFPT_APPLY_ACTIONS, 4, 0},
    {OFPTFPT_MATCH, 4, 0},
    {OFPTFPT_WILDCARDS, 4, 0},
    {OFPTFPT_WRITE_SETFIELD, 4, 0},
    {OFPTFPT_APPLY_SETFIELD, 4, 0},
};

bool ofpx::normalizeTableFeaturesMessage(const ByteRange &data,
                                         ByteList &results) {
  // Convert a multipart table features message into a standard form.

  if (!IsPtrAligned(data.data(), 8) || data.size() <= 16) {
    return false;
  }

  const UInt8 *ptr = data.data();
  if (ptr[0] < OFP_VERSION_4 ||
      (ptr[1] != OFPT_MULTIPART_REQUEST && ptr[1] != OFPT_MULTIPART_REPLY)) {
    return false;
  }

  if (data.size() != *Big16_cast(ptr + 2)) {
    return false;
  }

  UInt16 multipartType = *Big16_cast(ptr + 8);
  if (multipartType != OFPMP_TABLE_FEATURES) {
    return false;
  }

  size_t remainingLength = data.size() - 16;
  ptr += 16;

  log::debug("normalizeTableFeaturesMessage");
  results.add(data.data(), 16);

  while (remainingLength != 0) {
    if (!normalizeTableFeaturesEntry(ptr, remainingLength, results)) {
      log::error("Failed to normalize TableFeatures message");
      return false;
    }
  }

  // Fix up total message length.
  assert(results.size() < 65536);
  *Big16_cast(results.mutableData() + 2) = UInt16_narrow_cast(results.size());

  return true;
}

static bool normalizeTableFeaturesEntry(const UInt8 *&ptr,
                                        size_t &remainingLength,
                                        ByteList &results) {
  log::debug("normalizeTableFeaturesEntry");

  if (!IsPtrAligned(ptr, 8) || remainingLength < 64) {
    return false;
  }

  size_t entryLength = PadLength(*Big16_cast(ptr));
  if (entryLength < 64 || entryLength > remainingLength) {
    log::debug("bad entry length", entryLength);
    return false;
  }

  // Build list of pointers to properties so we can sort them.
  std::vector<const Prop *> props;

  const UInt8 *propPtr = ptr + 64;
  size_t left = entryLength - 64;

  while (left > 0) {
    if (left < 4) {
      log::debug("bad left", left);
      return false;
    }

    const Prop *p = reinterpret_cast<const Prop *>(propPtr);
    size_t offset = PadLength(p->length);

    if (offset < 4 || offset > left) {
      log::debug("bad offset", offset);
      return false;
    }

    log::debug("read property", left, ByteRange{p, offset});

    props.push_back(p);

    propPtr += offset;
    left -= offset;
  }

  // Add certain empty properties if they don't exist already.
  for (const EmptyProp &prop : kDefaultEmptyProperties) {
    // If property is not already in list, add it.
    auto iter =
        std::find_if(props.begin(), props.end(),
                     [&prop](const Prop *p) { return p->type == prop.type; });
    if (iter == props.end()) {
      props.push_back(reinterpret_cast<const Prop *>(&prop));
    }
  }

  // Sort the property pointers.
  std::sort(props.begin(), props.end(),
            [](const Prop *a, const Prop *b) { return (a->type < b->type); });

  size_t start = results.size();
  results.add(ptr, 64);

  const Prop *prev = nullptr;
  for (auto next : props) {
    size_t len = PadLength(next->length);

    if (prev && prev->length == next->length &&
        (prev->type & 0xFFFE) == (next->type & 0xFFFE) &&
        std::memcmp(&prev->length, &next->length, next->length - 2) == 0) {
      log::debug("Skip property", len, ByteRange{next, len});
      continue;
    }

    log::debug("Add property", len, ByteRange{next, len});
    results.add(next, len);
    prev = next;
  }

  assert(results.size() - start < 0xffff);
  *Big16_cast(results.mutableData() + start) =
      UInt16_narrow_cast(results.size() - start);

  remainingLength -= entryLength;
  ptr += entryLength;

  return true;
}
