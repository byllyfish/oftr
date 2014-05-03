#ifndef OFP_ACTIONID_H_
#define OFP_ACTIONID_H_

#include "ofp/constants.h"

namespace ofp {

class ActionID {
public:
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(OFPActionType),
    ProtocolIteratorAlignment = 4
  };
  
  explicit ActionID(OFPActionType type, UInt32 experimenter = 0)
      : type_(type, type == OFPAT_EXPERIMENTER ? 8U : 4U),
        experimenter_{experimenter} {}

  OFPActionType type() const { return type_.type(); }
  UInt32 experimenter() const { return experimenter_; }

private:
    ActionType type_;
    Big32 experimenter_;
};

}  // namespace ofp

#endif // OFP_ACTIONID_H_
