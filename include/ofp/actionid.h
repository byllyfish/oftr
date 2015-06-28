// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ACTIONID_H_
#define OFP_ACTIONID_H_

#include "ofp/constants.h"
#include "ofp/actiontype.h"

namespace ofp {

class ActionID {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(OFPActionType),
    ProtocolIteratorAlignment = 4
  };

  explicit ActionID(OFPActionType type = OFPAT_OUTPUT, UInt32 experimenter = 0)
      : type_(type, type == OFPAT_EXPERIMENTER ? 8U : 4U),
        experimenter_{experimenter} {}

  explicit ActionID(ActionType type, UInt32 experimenter = 0)
      : ActionID{type.enumType(), experimenter} {}

  ActionType type() const { return type_.zeroLength(); }
  UInt32 experimenter() const { return experimenter_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  ActionType type_;
  Big32 experimenter_;

  size_t length() const { return type_.length(); }

  friend class ActionIDList;
};

}  // namespace ofp

#endif  // OFP_ACTIONID_H_
