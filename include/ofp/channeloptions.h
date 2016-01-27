// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_CHANNELOPTIONS_H_
#define OFP_CHANNELOPTIONS_H_

namespace ofp {

/// FEATURES_REQ -- automatically send FeaturesRequest to obtain datapath_id
/// AUXILIARY    -- support auxiliary connections (requires FEATURE_REQ)
/// LISTEN_UDP   -- listen for UDP connections also (requires AUXILIARY)
/// CONNECT_UDP  -- connect over UDP *instead of* TCP (requires !FEATURE_REQ)

enum class ChannelOptions : UInt8 {
  NONE = 0,
  FEATURES_REQ = 1 << 0,
  AUXILIARY = 1 << 1,
  LISTEN_UDP = 1 << 2,
  CONNECT_UDP = 1 << 3,

  DEFAULT_CONTROLLER = FEATURES_REQ,
  DEFAULT_AGENT = NONE,
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
  // LISTEN_UDP requires AUXILIARY
  if ((options & ChannelOptions::LISTEN_UDP) != 0 &&
      (options & ChannelOptions::AUXILIARY) == 0)
    return false;
  // CONNECT_UDP requires !FEATURES_REQ
  if ((options & ChannelOptions::CONNECT_UDP) != 0 &&
      (options & ChannelOptions::FEATURES_REQ) != 0)
    return false;
  return true;
}

}  // namespace ofp

#endif  // OFP_CHANNELOPTIONS_H_
