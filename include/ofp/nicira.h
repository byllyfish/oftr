// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_NICIRA_H_
#define OFP_NICIRA_H_

#include "ofp/actiontype.h"
#include "ofp/oxmregister.h"
#include "ofp/validation.h"

namespace ofp {

class AT_EXPERIMENTER;

namespace nx {

enum NXActionSubtype : UInt16 {
  NXAST_REG_MOVE = 6,
  NXAST_REG_LOAD = 7,
};

const UInt32 NICIRA = 0x00002320;

/// \brief Concrete type for NXAST_REG_MOVE action.
class AT_REGMOVE {
 public:
  constexpr static ActionType type() {
    return ActionType(OFPAT_EXPERIMENTER, 24);
  }
  constexpr static UInt32 experimenter() { return NICIRA; }
  constexpr static UInt16 subtype() { return NXAST_REG_MOVE; }

  AT_REGMOVE(const OXMRegister &src, const OXMRegister &dst)
      : type_{type()},
        experimenterid_{experimenter()},
        subtype_{subtype()},
        nBits_{src.nbits()},
        srcOfs_{src.offset()},
        dstOfs_{dst.offset()},
        src_{src.type()},
        dst_{dst.type()} {}

  OXMRegister src() const { return OXMRegister{src_, srcOfs_, nBits_}; }

  OXMRegister dst() const { return OXMRegister{dst_, dstOfs_, nBits_}; }

  bool validateInput(Validation *context) const {
    return context->validateBool(type_.length() == 24,
                                 "Invalid AT_NX_REGMOVE action");
  }

  static const AT_REGMOVE *cast(const AT_EXPERIMENTER *action) {
    return reinterpret_cast<const AT_REGMOVE *>(action);
  }

 private:
  const ActionType type_;
  const Big32 experimenterid_;
  const Big16 subtype_;
  Big16 nBits_;
  Big16 srcOfs_;
  Big16 dstOfs_;
  OXMType src_;
  OXMType dst_;
};

static_assert(sizeof(AT_REGMOVE) == 24, "Unexpected size.");
static_assert(IsStandardLayout<AT_REGMOVE>(), "Unexpected layout");

/// \brief Concrete type for NXAST_REG_LOAD action.
class AT_REGLOAD {
public:
  constexpr static ActionType type() {
    return ActionType(OFPAT_EXPERIMENTER, 24);
  }
  constexpr static UInt32 experimenter() { return NICIRA; }
  constexpr static UInt16 subtype() { return NXAST_REG_LOAD; }

  AT_REGLOAD(UInt64 value, const OXMRegister &dst)
      : type_{type()},
        experimenterid_{experimenter()},
        subtype_{subtype()},
        ofsNbits_{UInt16_narrow_cast((dst.offset() << 6) | (dst.nbits() - 1))},
        dst_{dst.type()},
        value_{value} {}

  Big64 value() const { return value_; }

  OXMRegister dst() const { return OXMRegister{dst_, ofs(), nbits()}; }

  bool validateInput(Validation *context) const {
    return context->validateBool(type_.length() == 24,
                                 "Invalid AT_NX_REGLOAD action");
  }

  static const AT_REGLOAD *cast(const AT_EXPERIMENTER *action) {
    return reinterpret_cast<const AT_REGLOAD *>(action);
  }
private:
  const ActionType type_;
  const Big32 experimenterid_;
  const Big16 subtype_;
  Big16 ofsNbits_;
  OXMType dst_;
  Big64 value_;

  UInt16 ofs() const { return ofsNbits_ >> 6; }
  UInt16 nbits() const { return (ofsNbits_ & 0x03F) + 1; }
};

static_assert(sizeof(AT_REGLOAD) == 24, "Unexpected size.");
static_assert(IsStandardLayout<AT_REGLOAD>(), "Unexpected layout");

}  // namespace nx
}  // namespace ofp

#endif  // OFP_NICIRA_H_
