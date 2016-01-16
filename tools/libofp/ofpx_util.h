// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef TOOLS_LIBOFP_OFPX_UTIL_H_
#define TOOLS_LIBOFP_OFPX_UTIL_H_

#include "ofp/bytelist.h"

namespace ofpx {

bool normalizeTableFeaturesMessage(const ofp::ByteRange &data,
                                   ofp::ByteList &results);

}  // namespace ofpx

#endif  // TOOLS_LIBOFP_OFPX_UTIL_H_
