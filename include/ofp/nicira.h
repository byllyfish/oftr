#ifndef OFP_NICIRA_H_
#define OFP_NICIRA_H_

#include "ofp/oxmregister.h"
#include "ofp/validation.h"
#include "ofp/actiontype.h"

namespace ofp {
namespace nx {

enum NXActionSubtype : UInt16 {
  NXAST_REG_MOVE = 6,
  NXAST_REG_LOAD = 7,
};


const UInt32 NICIRA = 0x00002320;

/// \brief Concrete type for NXAST_REG_MOVE action.
class AT_REGMOVE {
 public:
  constexpr static ActionType type() { return ActionType(OFPAT_EXPERIMENTER, 24); }

  constexpr AT_REGMOVE(const OXMRegister &src, const OXMRegister &dst)
      : type_{type()},
        experimenterid_{NICIRA}, subtype_{NXAST_REG_MOVE}, nBits_{src.nbits()}, srcOfs_{src.offset()}, dstOfs_{dst.offset()}, src_{src.type()}, dst_{dst.type()} {
        }

  bool validateInput(Validation *context) const {
    return context->validateBool(type_.length() == 24,
                                 "Invalid AT_NX_REGMOVE action");
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

}  // namespace nx
}  // namespace ofp

#endif // OFP_NICIRA_H_
