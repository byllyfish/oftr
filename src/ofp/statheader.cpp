// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/statheader.h"
#include "ofp/validation.h"

using namespace ofp;

OXMRange StatHeader::oxmRange() const {
  assert(reserved_ == 0);
  assert(length() >= sizeof(StatHeader));
  return OXMRange{BytePtr(this) + sizeof(StatHeader),
                  length() - sizeof(StatHeader)};
}

bool StatHeader::validateInput(size_t lengthRemaining,
                               Validation *context) const {
  // Make sure remaining length at least accomodates the match header.
  if (lengthRemaining < sizeof(StatHeader)) {
    context->matchIsInvalid("Insufficient bytes", BytePtr(this));
    return false;
  }

  // Check alignment.
  if (!IsPtrAligned(this, 8)) {
    context->matchIsInvalid("Not aligned", BytePtr(this));
    return false;
  }

  // Check reserved.
  if (reserved_ != 0) {
    context->matchIsInvalid("Invalid reserved type", BytePtr(this));
    return false;
  }

  size_t statLength = length_;

  // Check statLength (which includes size of match header.)
  if (statLength < sizeof(StatHeader)) {
    context->matchIsInvalid("Invalid stat length", BytePtr(this));
    return false;
  }

  // Check statLength + padding against remaining length.
  size_t statLengthPadded = PadLength(statLength);
  if (statLengthPadded > lengthRemaining) {
    context->matchIsInvalid("Invalid remaining length", BytePtr(this));
    return false;
  }

  // Check the stat contents.
  OXMRange oxm{BytePtr(this) + sizeof(StatHeader),
               statLength - sizeof(StatHeader)};
  if (!oxm.validateInput()) {
    context->matchIsInvalid("Invalid oxm range", BytePtr(this));
    return false;
  }

  return true;
}
