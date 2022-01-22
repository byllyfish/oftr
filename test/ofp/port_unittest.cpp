// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/port.h"

#include "ofp/portproperty.h"
#include "ofp/unittest.h"

using namespace ofp;

const OFPPortConfigFlags kFakeConfig =
    static_cast<OFPPortConfigFlags>(0x22222222);
const OFPPortStateFlags kFakeState = static_cast<OFPPortStateFlags>(0x33333333);
const OFPPortFeaturesFlags kFakeCurr =
    static_cast<OFPPortFeaturesFlags>(0x44444444);
const OFPPortFeaturesFlags kFakeAdvertised =
    static_cast<OFPPortFeaturesFlags>(0x55555555);
const OFPPortFeaturesFlags kFakeSupported =
    static_cast<OFPPortFeaturesFlags>(0x66666666);
const OFPPortFeaturesFlags kFakePeer =
    static_cast<OFPPortFeaturesFlags>(0x77777777);

TEST(port, test) {
  PortBuilder portBuilder;

  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(MacAddress{"010203040506"});
  portBuilder.setName("Port 1");
  portBuilder.setConfig(kFakeConfig);
  portBuilder.setState(kFakeState);

  PortPropertyEthernet prop;
  prop.setCurr(kFakeCurr);
  prop.setAdvertised(kFakeAdvertised);
  prop.setSupported(kFakeSupported);
  prop.setPeer(kFakePeer);
  prop.setCurrSpeed(0x88888888);
  prop.setMaxSpeed(0x99999999);

  PropertyList properties;
  properties.add(prop);
  portBuilder.setProperties(properties);

  MemoryChannel channel{OFP_VERSION_5};
  portBuilder.write(&channel);

  EXPECT_HEX(
      "11111111004800000102030405060000506F727420310000000000000000000022222222"
      "33333333000000200000000044444444555555556666666677777777888888889999999"
      "9",
      channel.data(), channel.size());
}
