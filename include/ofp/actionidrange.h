// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ACTIONIDRANGE_H_
#define OFP_ACTIONIDRANGE_H_

#include "ofp/actionid.h"
#include "ofp/protocolrange.h"

namespace ofp {

using ActionIDIterator =
    ProtocolIterator<ActionID, ProtocolIteratorType::ActionID>;
using ActionIDRange = ProtocolRange<ActionIDIterator>;

}  // namespace ofp

#endif  // OFP_ACTIONIDRANGE_H_
