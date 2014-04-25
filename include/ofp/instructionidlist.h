#ifndef OFP_INSTRUCTIONIDLIST_H_
#define OFP_INSTRUCTIONIDLIST_H_

#include "ofp/protocollist.h"
#include "ofp/instructionidrange.h"

namespace ofp {

class InstructionIDList : public ProtocolList<InstructionIDRange> {
public:

};

}  // namespace ofp

#endif // OFP_INSTRUCTIONIDLIST_H_
