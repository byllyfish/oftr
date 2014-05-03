#ifndef OFP_OXMIDRANGE_H_
#define OFP_OXMIDRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/oxmid.h"

namespace ofp {

using OXMIDIterator = ProtocolIterator<OXMID>;
using OXMIDRange = ProtocolRange<OXMIDIterator>;

}  // namespace ofp

#endif // OFP_OXMIDRANGE_H_
