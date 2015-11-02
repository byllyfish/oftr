#ifndef TOOLS_LIBOFP_OFPX_UTIL_H_
#define TOOLS_LIBOFP_OFPX_UTIL_H_

#include "ofp/bytelist.h"

namespace ofpx {

bool normalizeTableFeaturesMessage(const ofp::ByteRange &data, ofp::ByteList &results);

}  // namespace ofpx

#endif // TOOLS_LIBOFP_OFPX_UTIL_H_
