// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/port.h"
#include "ofp/portproperty.h"

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

#if 0
  const Port &port = portBuilder.toPort();
  EXPECT_EQ(0x11111111, port.portNo());
  EXPECT_EQ(MacAddress{"010203040506"}, port.hwAddr());
  EXPECT_EQ(PortNameStr{"Port 1"}, port.name());
  EXPECT_EQ(0x22222222, port.config());
  EXPECT_EQ(0x33333333, port.state());
  EXPECT_EQ(0x44444444, port.curr());
  EXPECT_EQ(0x55555555, port.advertised());
  EXPECT_EQ(0x66666666, port.supported());
  EXPECT_EQ(0x77777777, port.peer());
  EXPECT_EQ(0x88888888, port.currSpeed());
  EXPECT_EQ(0x99999999, port.maxSpeed());

  deprecated::PortV1 portv1{port};

  EXPECT_HEX(
      "1111010203040506506F727420310000000000000000000022222222333333334"
      "4440444555505556666066677770777",
      &portv1, sizeof(portv1));

  EXPECT_EQ(0x1111, portv1.portNo());
  EXPECT_EQ(MacAddress{"010203040506"}, portv1.hwAddr());
  EXPECT_EQ(PortNameStr{"Port 1"}, portv1.name());
  EXPECT_EQ(0x22222222, portv1.config());
  EXPECT_EQ(0x33333333, portv1.state());
  EXPECT_EQ(0x44444044, portv1.curr());
  EXPECT_EQ(0x55555055, portv1.advertised());
  EXPECT_EQ(0x66666066, portv1.supported());
  EXPECT_EQ(0x77777077, portv1.peer());

  PortBuilder port2Builder{portv1};
  const Port &port2 = port2Builder.toPort();

  EXPECT_EQ(0x1111, port2.portNo());
  EXPECT_EQ(MacAddress{"010203040506"}, port2.hwAddr());
  EXPECT_EQ(PortNameStr{"Port 1"}, port2.name());
  EXPECT_EQ(0x22222222, port2.config());
  EXPECT_EQ(0x33333333, port2.state());
  EXPECT_EQ(0x44444044, port2.curr());
  EXPECT_EQ(0x55555055, port2.advertised());
  EXPECT_EQ(0x66666066, port2.supported());
  EXPECT_EQ(0x77777077, port2.peer());
  EXPECT_EQ(0, port2.currSpeed());
  EXPECT_EQ(0, port2.maxSpeed());
#endif  // 0
}
