// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ACTIONIDRANGE_H_
#define OFP_ACTIONIDRANGE_H_

#include "ofp/protocolrange.h"
#include "ofp/actionid.h"

namespace ofp {

using ActionIDIterator = ProtocolIterator<ActionID, ProtocolIteratorType::ActionID>;
using ActionIDRange = ProtocolRange<ActionIDIterator>;

}  // namespace ofp

#endif  // OFP_ACTIONIDRANGE_H_
