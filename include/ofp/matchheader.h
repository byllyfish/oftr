#ifndef OFP_MATCHHEADER_H_
#define OFP_MATCHHEADER_H_

#include "ofp/constants.h"
#include "ofp/byteorder.h"
#include "ofp/oxmrange.h"
#include "ofp/standardmatch.h"

namespace ofp {

class MatchHeader {
 public:
  MatchHeader() = default;

  UInt16 type() const { return type_; }
  void setType(UInt16 type) { type_ = type; }

  UInt16 length() const { return length_; }
  void setLength(size_t length) { length_ = UInt16_narrow_cast(length); }

  size_t paddedLength() const { return PadLength(length_); }
  OXMRange oxmRange() const;
  const deprecated::StandardMatch *stdMatch() const;

  bool validateInput(size_t lengthRemaining) const;

 private:
  Big16 type_ = OFPMT_OXM;
  Big16 length_ = 0;
};

static_assert(sizeof(MatchHeader) == 4, "Unexpected size.");
static_assert(IsStandardLayout<MatchHeader>(), "Expected standard layout.");

}  // namespace ofp

#endif  // OFP_MATCHHEADER_H_
