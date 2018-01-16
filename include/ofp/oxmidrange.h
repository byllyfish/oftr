// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_OXMIDRANGE_H_
#define OFP_OXMIDRANGE_H_

#include "ofp/oxmid.h"
#include "ofp/protocolrange.h"

namespace ofp {

using OXMIDIterator = ProtocolIterator<OXMID, ProtocolIteratorType::OXMID>;
using OXMIDRange = ProtocolRange<OXMIDIterator>;

}  // namespace ofp

#endif  // OFP_OXMIDRANGE_H_
