// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchheader.h"
#include "ofp/oxmrange.h"
#include "ofp/padding.h"
#include "ofp/validation.h"

using namespace ofp;

OXMRange MatchHeader::oxmRange() const {
  assert(length() >= sizeof(MatchHeader));
  if (type() != OFPMT_OXM)
    return {};
  return OXMRange{BytePtr(this) + sizeof(MatchHeader),
                  length() - sizeof(MatchHeader)};
}

const deprecated::StandardMatch *MatchHeader::stdMatch() const {
  assert(type() == OFPMT_STANDARD);
  assert(length() == deprecated::OFPMT_STANDARD_LENGTH);
  return reinterpret_cast<const deprecated::StandardMatch *>(this);
}

bool MatchHeader::validateInput(size_t lengthRemaining,
                                Validation *context) const {
  // Make sure remaining length at least accomodates the match header.
  if (lengthRemaining < sizeof(MatchHeader)) {
    context->matchIsInvalid("Insufficient bytes", BytePtr(this));
    return false;
  }

  // Check alignment.
  if (!IsPtrAligned(this, 8)) {
    context->matchIsInvalid("Not aligned", BytePtr(this));
    return false;
  }

  UInt16 matchType = type_;
  size_t matchLength = length_;

  if (context->version() < OFP_VERSION_4) {
    // Special case for standard match; verify match length.
    if (matchType == OFPMT_STANDARD) {
      if (matchLength != deprecated::OFPMT_STANDARD_LENGTH) {
        context->matchIsInvalid("Invalid match length", BytePtr(this));
        return false;
      }
      return true;
    }
  }

  // Check matchType.
  if (matchType != OFPMT_OXM) {
    context->matchIsInvalid("Invalid match type", BytePtr(this));
    return false;
  }

  // Check matchLength (which includes size of match header.)
  if (matchLength < sizeof(MatchHeader)) {
    context->matchIsInvalid("Invalid match length", BytePtr(this));
    return false;
  }

  // Check matchLength + padding against remaining length.
  size_t matchLengthPadded = PadLength(matchLength);
  if (matchLengthPadded > lengthRemaining) {
    context->matchIsInvalid("Invalid remaining length", BytePtr(this));
    return false;
  }

  // Check the match contents.
  OXMRange oxm{BytePtr(this) + sizeof(MatchHeader),
               matchLength - sizeof(MatchHeader)};
  if (!oxm.validateInput()) {
    context->matchIsInvalid("Invalid oxm range", BytePtr(this));
    return false;
  }

  return true;
}
