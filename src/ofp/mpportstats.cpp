// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/mpportstats.h"
#include "ofp/writable.h"
#include "ofp/constants.h"
#include "ofp/portstatsproperty.h"
#include "ofp/validation.h"

using namespace ofp;

PropertyRange MPPortStats::properties() const {
  assert(length_ >= sizeof(MPPortStats));
  return ByteRange{BytePtr(this) + sizeof(MPPortStats),
                   length_ - sizeof(MPPortStats)};
}

bool MPPortStats::validateInput(Validation *context) const {
  if (context->lengthRemaining() < sizeof(MPPortStats)) {
    return false;
  }

  return properties().validateInput(context);
}

void MPPortStatsBuilder::write(Writable *channel) {
  UInt8 version = channel->version();

  if (version >= OFP_VERSION_5) {
    channel->write(&msg_, sizeof(msg_));
    channel->write(properties_.data(), properties_.size());

  } else {
    if (version == OFP_VERSION_1) {
      struct {
        Big16 portNo;
        Padding<6> pad;
      } p;
      p.portNo = UInt16_narrow_cast(msg_.portNo_);
      channel->write(&p, sizeof(p));

    } else {
      struct {
        Big32 portNo;
        Padding<4> pad;
      } p;
      p.portNo = msg_.portNo_;
      channel->write(&p, sizeof(p));
    }

    channel->write(&msg_.rxPackets_, 64);

    PropertyRange props = properties_.toRange();
    auto iter = props.findProperty(PortStatsPropertyEthernet::type());
    if (iter != props.end()) {
      const PortStatsPropertyEthernet &eth =
          iter->property<PortStatsPropertyEthernet>();
      channel->write(&eth.rxFrameErr_, 32);
    } else {
      Padding<32> pad;
      channel->write(&pad, sizeof(pad));
    }

    if (version == OFP_VERSION_4) {
      channel->write(&msg_.durationSec_, 8);
    }
  }

  channel->flush();

  /**
    if (version == OFP_VERSION_1) {
      struct {
        Big16 portNo;
        Padding<6> pad;
      } p;
      p.portNo = UInt16_narrow_cast(msg_.portNo_);

      channel->write(&p, sizeof(p));
      channel->write(&msg_.rxPackets_, 8 * 12);
      channel->flush();

    } else if (version == OFP_VERSION_3 || version == OFP_VERSION_2) {
      channel->write(&msg_, 104);
      channel->flush();

    } else {
      channel->write(&msg_, sizeof(msg_));
      channel->flush();
    }
    */
}
