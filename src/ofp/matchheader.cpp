// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchheader.h"
#include "ofp/padding.h"
#include "ofp/oxmrange.h"

using namespace ofp;

OXMRange MatchHeader::oxmRange() const {
  assert(type() == OFPMT_OXM);
  assert(length() >= sizeof(MatchHeader));
  return OXMRange{BytePtr(this) + sizeof(MatchHeader),
                  length() - sizeof(MatchHeader)};
}

const deprecated::StandardMatch *MatchHeader::stdMatch() const {
  assert(type() == OFPMT_STANDARD);
  assert(length() == deprecated::OFPMT_STANDARD_LENGTH);
  return reinterpret_cast<const deprecated::StandardMatch *>(this);
}

bool MatchHeader::validateInput(size_t lengthRemaining) const {
  // Make sure remaining length at least accomodates the match header.
  if (lengthRemaining < sizeof(MatchHeader)) {
    return false;
  }

  // Check alignment.
  if (!IsPtrAligned(this, 8)) {
    log::debug("MatchHeader: Not aligned.");
    return false;
  }

  UInt16 matchType = type_;
  size_t matchLength = length_;

  // Special case for standard match; verify match length.
  if (matchType == OFPMT_STANDARD) {
    if (matchLength != deprecated::OFPMT_STANDARD_LENGTH) {
      log::debug("MatchHeader: Invalid match length.");
      return false;
    }
    return true;
  }

  // Check matchType.
  if (matchType != OFPMT_OXM) {
    log::debug("MatchHeader: Invalid match type.", matchType);
    return false;
  }

  // Check matchLength (which includes size of match header.)
  if (matchLength < sizeof(MatchHeader)) {
    log::debug("MatchHeader: Invalid match length.");
    return false;
  }

  // Check matchLength + padding against remaining length.
  size_t matchLengthPadded = PadLength(matchLength);
  if (matchLengthPadded > lengthRemaining) {
    log::debug("MatchHeader: Invalid remaining length.");
    return false;
  }

  // Check the match contents.
  OXMRange oxm{BytePtr(this) + sizeof(MatchHeader),
               matchLength - sizeof(MatchHeader)};
  if (!oxm.validateInput()) {
    log::debug("MatchHeader: Invalid oxm range.");
    return false;
  }

  // Check that the padding bytes (if present) are all zeros.
  if (matchLengthPadded > matchLength) {
    if (!IsMemFilled(BytePtr(this) + matchLength,
                     matchLengthPadded - matchLength, 0)) {
      log::debug("MatchHeader: Invalid padding.");
      return false;
    }
  }

  return true;
}
