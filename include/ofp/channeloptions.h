// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_CHANNELOPTIONS_H_
#define OFP_CHANNELOPTIONS_H_

#include "ofp/types.h"

namespace ofp {

/// FEATURES_REQ -- automatically send FeaturesRequest to obtain datapath_id
/// AUXILIARY    -- support auxiliary connections (requires FEATURE_REQ)
/// NO_VERSION_CHECK -- allow versions other than the one negotiated with HELLO

enum class ChannelOptions : UInt8 {
  NONE = 0,
  FEATURES_REQ = 1 << 0,
  AUXILIARY = 1 << 1,
  NO_VERSION_CHECK = 1 << 2
};

constexpr ChannelOptions operator&(ChannelOptions lhs, ChannelOptions rhs) {
  return static_cast<ChannelOptions>(static_cast<unsigned>(lhs) &
                                     static_cast<unsigned>(rhs));
}

constexpr ChannelOptions operator|(ChannelOptions lhs, ChannelOptions rhs) {
  return static_cast<ChannelOptions>(static_cast<unsigned>(lhs) |
                                     static_cast<unsigned>(rhs));
}

constexpr bool operator==(ChannelOptions lhs, unsigned rhs) {
  return static_cast<unsigned>(lhs) == rhs;
}

constexpr bool operator!=(ChannelOptions lhs, unsigned rhs) {
  return !operator==(lhs, rhs);
}

inline bool AreChannelOptionsValid(ChannelOptions options) {
  // AUXILIARY requires FEATURES_REQ
  if ((options & ChannelOptions::AUXILIARY) != 0 &&
      (options & ChannelOptions::FEATURES_REQ) == 0)
    return false;
  return true;
}

}  // namespace ofp

#endif  // OFP_CHANNELOPTIONS_H_
