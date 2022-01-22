// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/portmod.h"

#include "ofp/portmodproperty.h"
#include "ofp/writable.h"

using namespace ofp;

PropertyRange PortMod::properties() const {
  return SafeByteRange(this, header_.length(), sizeof(PortMod));
}

bool PortMod::validateInput(Validation *context) const {
  return properties().validateInput(context);
}

PortModBuilder::PortModBuilder(const PortMod *msg) : msg_{*msg} {}

UInt32 PortModBuilder::send(Writable *channel) {
  UInt8 version = channel->version();
  UInt32 xid = channel->nextXid();

  msg_.header_.setVersion(version);
  msg_.header_.setXid(xid);

  if (version >= OFP_VERSION_5) {
    size_t msgLen = sizeof(msg_) + properties_.size();
    msg_.header_.setLength(msgLen);

    channel->write(&msg_, sizeof(msg_));
    channel->write(properties_.data(), properties_.size());
    channel->flush();
    return xid;
  }

  // For OpenFlow version 1.3 and previous, only the Ethernet property
  // should be the present.

  if (properties_.size() != sizeof(PortModPropertyEthernet)) {
    log_warning("PortModBuilder: Invalid properties for version", version);
  }

  OFPPortFeaturesFlags advertise =
      properties_.toRange().value<PortModPropertyEthernet>();

  if (version >= OFP_VERSION_2) {
    size_t msgLen = sizeof(msg_) + sizeof(OFPPortFeaturesFlags) + 4;
    msg_.header_.setLength(msgLen);
    channel->write(&msg_, sizeof(msg_));

    Big<OFPPortFeaturesFlags> adv = advertise;
    Padding<4> pad;
    channel->write(&adv, sizeof(adv));
    channel->write(&pad, sizeof(pad));

  } else {
    size_t msgLen = sizeof(msg_);
    // Temporarily set the message type to the V1 value.
    msg_.header_.setRawType(deprecated::v1::OFPT_PORT_MOD);
    msg_.header_.setLength(msgLen);
    channel->write(&msg_.header_, sizeof(Header));
    msg_.header_.setType(OFPT_PORT_MOD);

    channel->write(BytePtr(&msg_.portNo_) + 2, 2);
    channel->write(&msg_.hwAddr_, 6);
    channel->write(&msg_.config_, 8);

    Big<OFPPortFeaturesFlags> adv = OFPPortFeaturesFlagsConvertToV1(advertise);
    Padding<4> pad;
    channel->write(&adv, sizeof(adv));
    channel->write(&pad, sizeof(pad));
  }

  channel->flush();

  return xid;
}
