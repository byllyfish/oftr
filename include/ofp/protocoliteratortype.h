#ifndef OFP_PROTOCOLITERATORTYPE_H_
#define OFP_PROTOCOLITERATORTYPE_H_

#include "ofp/types.h"

namespace ofp {

enum class ProtocolIteratorType {
    Unspecified,
    ActionID,
    Action,
    Bucket,
    HelloElement,
    InstructionID,
    Instruction,
    MeterBand,
    OXMID,
    // OXM itself is implemented by OXMIterator, not a ProtocolIterator.
    PacketCounter,
    Port,
    Property,
    Queue
};

}  // namespace ofp

#endif // OFP_PROTOCOLITERATORTYPE_H_
