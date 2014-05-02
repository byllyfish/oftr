#ifndef OFP_ACTIONIDRANGE_H_
#define OFP_ACTIONIDRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/actionid.h"

namespace ofp {

using ActionIDIterator = ProtocolIterator<ActionID, ActionID::ProtocolIteratorSizeOffset, 4>;
using ActionIDRange = ProtocolRange<ActionIDIterator>;

}  // namespace ofp

#endif // OFP_ACTIONIDRANGE_H_
