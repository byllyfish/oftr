// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/portstatus.h"
#include "ofp/message.h"

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

const OFPPortStatusReason kFakeReason = static_cast<OFPPortStatusReason>(0x33);

TEST(portstatus, v4) {
  PortBuilder port;
  port.setPortNo(0x11111111);
  port.setHwAddr(EnetAddress{"010203040506"});
  port.setName("Port 1");
  port.setConfig(kFakeConfig);
  port.setState(kFakeState);
  port.setCurr(kFakeCurr);
  port.setAdvertised(kFakeAdvertised);
  port.setSupported(kFakeSupported);
  port.setPeer(kFakePeer);
  port.setCurrSpeed(0x88888888);
  port.setMaxSpeed(0x99999999);

  PortStatusBuilder builder;
  builder.setReason(kFakeReason);
  builder.setPort(port);

  MemoryChannel channel{OFP_VERSION_4};
  UInt32 xid = builder.send(&channel);

  EXPECT_EQ(1, xid);

  EXPECT_EQ(0x50, channel.size());
  EXPECT_HEX(
      "040C00500000000133000000000000001111111100000000010203040506000"
      "0506F7274203100000000000000000000222222223333333344444444555555"
      "5566666666777777778888888899999999",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const PortStatus *msg = PortStatus::cast(&message);
  EXPECT_TRUE(msg);

  if (msg) {
    EXPECT_EQ(0x33, msg->reason());

    const Port &p = msg->port();
    EXPECT_EQ(0x11111111, p.portNo());
    EXPECT_EQ(EnetAddress{"010203040506"}, p.hwAddr());
    EXPECT_EQ(PortNameStr{"Port 1"}, p.name());
    EXPECT_EQ(0x22222222, p.config());
    EXPECT_EQ(0x33333333, p.state());
    EXPECT_EQ(0x44444444, p.curr());
    EXPECT_EQ(0x55555555, p.advertised());
    EXPECT_EQ(0x66666666, p.supported());
    EXPECT_EQ(0x77777777, p.peer());
    EXPECT_EQ(0x88888888, p.currSpeed());
    EXPECT_EQ(0x99999999, p.maxSpeed());
  }
}

TEST(portstatus, v1) {
  PortBuilder port;
  port.setPortNo(0x11111111);
  port.setHwAddr(EnetAddress{"010203040506"});
  port.setName("Port 1");
  port.setConfig(kFakeConfig);
  port.setState(kFakeState);
  port.setCurr(kFakeCurr);
  port.setAdvertised(kFakeAdvertised);
  port.setSupported(kFakeSupported);
  port.setPeer(kFakePeer);
  port.setCurrSpeed(0x88888888);
  port.setMaxSpeed(0x99999999);

  PortStatusBuilder builder;
  builder.setReason(kFakeReason);
  builder.setPort(port);

  MemoryChannel channel{OFP_VERSION_1};
  UInt32 xid = builder.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x40, channel.size());
  EXPECT_HEX(
      "010C00400000000133000000000000001111010203040506506F72742031000"
      "000000000000000002222222233333333444404445555055566660666777707"
      "77",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const PortStatus *msg = PortStatus::cast(&message);
  EXPECT_TRUE(msg);

  if (msg) {
    EXPECT_EQ(0x33, msg->reason());

    const Port &p = msg->port();
    EXPECT_EQ(0x1111, p.portNo());
    EXPECT_EQ(EnetAddress{"010203040506"}, p.hwAddr());
    EXPECT_EQ(PortNameStr{"Port 1"}, p.name());
    EXPECT_EQ(0x22222222, p.config());
    EXPECT_EQ(0x33333333, p.state());
    EXPECT_EQ(0x44444044, p.curr());
    EXPECT_EQ(0x55555055, p.advertised());
    EXPECT_EQ(0x66666066, p.supported());
    EXPECT_EQ(0x77777077, p.peer());
    EXPECT_EQ(0, p.currSpeed());
    EXPECT_EQ(0, p.maxSpeed());
  }
}
