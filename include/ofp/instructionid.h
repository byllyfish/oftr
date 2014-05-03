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
  
  explicit InstructionID(OFPInstructionType type, UInt32 experimenter = 0)
      : type_{type},
        len_(type == OFPIT_EXPERIMENTER ? 8U : 4U),
        experimenter_{experimenter} {}

  InstructionType type() const { return type_; }
  UInt32 experimenter() const { return experimenter_; }

 private:
  InstructionType type_;
  Big16 len_;
  Big32 experimenter_;
};

}  // namespace ofp

#endif  // OFP_INSTRUCTIONID_H_
