// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef TOOLS_OFTR_OFTR_UTIL_H_
#define TOOLS_OFTR_OFTR_UTIL_H_

#include "ofp/bytelist.h"

namespace ofpx {

bool normalizeTableFeaturesMessage(const ofp::ByteRange &data,
                                   ofp::ByteList &results);

}  // namespace ofpx

#endif  // TOOLS_OFTR_OFTR_UTIL_H_
