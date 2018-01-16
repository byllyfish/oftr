// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_STATHEADER_H_
#define OFP_STATHEADER_H_

#include "ofp/byteorder.h"
#include "ofp/oxmrange.h"

namespace ofp {

class Validation;

class StatHeader {
 public:
  StatHeader() = default;

  UInt16 length() const { return length_; }
  void setLength(size_t length) { length_ = UInt16_narrow_cast(length); }

  size_t paddedLength() const { return PadLength(length_); }
  OXMRange oxmRange() const;

  bool validateInput(size_t lengthRemaining, Validation *context) const;

 private:
  Big16 reserved_ = 0;
  Big16 length_ = 0;
};

static_assert(sizeof(StatHeader) == 4, "Unexpected size.");
static_assert(IsStandardLayout<StatHeader>(), "Expected standard layout.");

}  // namespace ofp

#endif  // OFP_STATHEADER_H_
