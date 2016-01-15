// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_INSTRUCTIONID_H_
#define OFP_INSTRUCTIONID_H_

#include "ofp/instructiontype.h"

namespace ofp {

class InstructionID {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(InstructionType),
    ProtocolIteratorAlignment = 4
  };

  explicit InstructionID(OFPInstructionType type = OFPIT_GOTO_TABLE,
                         UInt32 experimenter = 0)
      : type_{type},
        len_(type == OFPIT_EXPERIMENTER ? 8U : 4U),
        experimenter_{experimenter} {}

  InstructionType type() const { return type_; }
  UInt32 experimenter() const { return experimenter_; }

  bool validateInput(Validation *context) const { return true; }

 private:
  InstructionType type_;
  Big16 len_;
  Big32 experimenter_;

  size_t length() const { return len_; }

  friend class InstructionIDList;
};

}  // namespace ofp

#endif  // OFP_INSTRUCTIONID_H_
