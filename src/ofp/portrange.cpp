// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/portrange.h"
#include "ofp/portlist.h"
#include "ofp/writable.h"

using namespace ofp;

PortRange::PortRange(const PortList &ports) : PortRange{ports.toRange()} {}

/// \returns Size of port list when written to channel using the specified
/// protocol version.
size_t PortRange::writeSize(Writable *channel) {
  if (channel->version() >= OFP_VERSION_5) {
    return size();
  }
  if (channel->version() >= OFP_VERSION_2) {
    return itemCount() * sizeof(deprecated::PortV2);
  }
  return itemCount() * sizeof(deprecated::PortV1);
}

/// \brief Writes port list to the channel using the specified protocol
/// version.
void PortRange::write(Writable *channel) {
  if (channel->version() >= OFP_VERSION_5) {
    channel->write(data(), size());

  } else if (channel->version() >= OFP_VERSION_2) {
    // Version 2-4 uses the PortV2 structure instead.
    for (auto &item : *this) {
      deprecated::PortV2 portV2{item};
      channel->write(&portV2, sizeof(portV2));
    }
  } else {
    // Version 1 uses the PortV1 structure instead.
    for (auto &item : *this) {
      deprecated::PortV1 portV1{item};
      channel->write(&portV1, sizeof(portV1));
    }
  }
}
