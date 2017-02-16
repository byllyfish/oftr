#ifndef OFP_MESSAGETYPE_H_
#define OFP_MESSAGETYPE_H_

#include "ofp/constants.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class MessageType {
public:
  MessageType() = default;
  explicit MessageType(OFPType t, OFPMultipartType mt = OFPMP_UNSUPPORTED) : type_{t}, subtype_{mt} {}

  OFPType type() const { return type_; }
  OFPMultipartType subtype() const { return subtype_; }

private:
  OFPType type_ = OFPT_UNSUPPORTED;
  OFPMultipartType subtype_ = OFPMP_UNSUPPORTED;

  template <class T>
  friend struct llvm::yaml::ScalarTraits;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif // OFP_MESSAGETYPE_H_
