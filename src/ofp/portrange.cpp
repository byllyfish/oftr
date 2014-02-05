#include "ofp/portrange.h"
#include "ofp/portlist.h"
#include "ofp/writable.h"

using ofp::PortRange;

PortRange::PortRange(const PortList &ports) : PortRange{ports.toRange()} {}

/// \returns Size of port list when written to channel using the specified
/// protocol version.
size_t PortRange::writeSize(Writable *channel) {
    if (channel->version() == OFP_VERSION_1) {
        return itemCount() * sizeof(deprecated::PortV1);
    } else {
        return size();
    }
}

/// \brief Writes port list to the channel using the specified protocol
/// version.
void PortRange::write(Writable *channel) {

  if (channel->version() == OFP_VERSION_1) {
    // Version 1 uses the PortV1 structure instead.
    for (auto item : *this) {
      deprecated::PortV1 portV1{item};
      channel->write(&portV1, sizeof(portV1));
    }

  } else {
    channel->write(data(), size());
  }
}
