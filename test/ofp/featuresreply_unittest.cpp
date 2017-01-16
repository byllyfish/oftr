// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/featuresreply.h"
#include "ofp/portproperty.h"
#include "ofp/unittest.h"

using namespace ofp;

const OFPPortConfigFlags kFakeConfig =
    static_cast<OFPPortConfigFlags>(0x44444444);
const OFPPortStateFlags kFakeState = static_cast<OFPPortStateFlags>(0x55555555);

const OFPPortFeaturesFlags kFakeCurr =
    static_cast<OFPPortFeaturesFlags>(0x66666666);
const OFPPortFeaturesFlags kFakeAdvertised =
    static_cast<OFPPortFeaturesFlags>(0x77777777);
const OFPPortFeaturesFlags kFakeSupported =
    static_cast<OFPPortFeaturesFlags>(0x88888888);
const OFPPortFeaturesFlags kFakePeer =
    static_cast<OFPPortFeaturesFlags>(0x99999999);

const OFPActionTypeFlags kFakeActions =
    static_cast<OFPActionTypeFlags>(0x77777777);
const OFPCapabilitiesFlags kFakeCapabilities =
    static_cast<OFPCapabilitiesFlags>(0x66666666);

TEST(featuresreply, v4) {
  PortBuilder portBuilder;
  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(MacAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(kFakeConfig);
  portBuilder.setState(kFakeState);

  PortPropertyEthernet prop;
  prop.setCurr(kFakeCurr);
  prop.setAdvertised(kFakeAdvertised);
  prop.setSupported(kFakeSupported);
  prop.setPeer(kFakePeer);
  prop.setCurrSpeed(0xAAAAAAAA);
  prop.setMaxSpeed(0xBBBBBBBB);

  PropertyList properties;
  properties.add(prop);
  portBuilder.setProperties(properties);

  PortList ports;
  ports.add(portBuilder);
  ports.add(portBuilder);

  FeaturesReplyBuilder msg{1};
  msg.setDatapathId(DatapathID("2222222222222222"));
  msg.setBufferCount(0x33333333);
  msg.setTableCount(0x44);
  msg.setAuxiliaryId(0x55);
  msg.setCapabilities(kFakeCapabilities);
  msg.setActions(kFakeActions);
  msg.setPorts(ports);

  MemoryChannel channel{OFP_VERSION_4};
  UInt32 xid = msg.send(&channel);

  // N.B. OpenFlow 1.3 and above do not include port list in feature reply.
  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x20, channel.size());
  EXPECT_HEX("0406002000000001222222222222222233333333445500006666666600000000",
             channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.normalize();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  ASSERT_TRUE(reply);

  EXPECT_EQ(DatapathID("2222222222222222"), reply->datapathId());
  EXPECT_EQ(0x33333333, reply->bufferCount());
  EXPECT_EQ(0x44, reply->tableCount());
  EXPECT_EQ(0x55, reply->auxiliaryId());
  EXPECT_EQ(0x66666666, reply->capabilities());
  EXPECT_EQ(0, reply->actions());

  EXPECT_EQ(0, reply->ports().size());
}

TEST(featuresreply, v3) {
  PortBuilder portBuilder;
  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(MacAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(kFakeConfig);
  portBuilder.setState(kFakeState);

  PortPropertyEthernet prop;
  prop.setCurr(kFakeCurr);
  prop.setAdvertised(kFakeAdvertised);
  prop.setSupported(kFakeSupported);
  prop.setPeer(kFakePeer);
  prop.setCurrSpeed(0xAAAAAAAA);
  prop.setMaxSpeed(0xBBBBBBBB);

  PropertyList properties;
  properties.add(prop);
  portBuilder.setProperties(properties);

  PortList ports;
  ports.add(portBuilder);
  ports.add(portBuilder);

  FeaturesReplyBuilder msg{1};
  msg.setDatapathId(DatapathID("2222222222222222"));
  msg.setBufferCount(0x33333333);
  msg.setTableCount(0x44);
  msg.setAuxiliaryId(0x55);
  msg.setCapabilities(kFakeCapabilities);
  msg.setActions(kFakeActions);
  msg.setPorts(ports);

  MemoryChannel channel{OFP_VERSION_3};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0xA0, channel.size());
  EXPECT_HEX(
      "030600A000000001222222222222222233333333440000006666666600000000"
      "11111111000000002222222222220000506F7274203300000000000000000000"
      "444444445555555566666666777777778888888899999999AAAAAAAABBBBBBBB"
      "11111111000000002222222222220000506F7274203300000000000000000000"
      "444444445555555566666666777777778888888899999999AAAAAAAABBBBBBBB",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.normalize();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  ASSERT_TRUE(reply);

  EXPECT_EQ(DatapathID("2222222222222222"), reply->datapathId());
  EXPECT_EQ(0x33333333, reply->bufferCount());
  EXPECT_EQ(0x44, reply->tableCount());
  EXPECT_EQ(0, reply->auxiliaryId());
  EXPECT_EQ(0x66666666, reply->capabilities());
  EXPECT_EQ(0, reply->actions());

  EXPECT_EQ(2, reply->ports().itemCount());

  for (auto &p : reply->ports()) {
    EXPECT_EQ(0x11111111, p.portNo());
    EXPECT_EQ(MacAddress{"22-22-22-22-22-22"}, p.hwAddr());
    EXPECT_EQ(SmallCString<16>("Port 3"), p.name());
    EXPECT_EQ(kFakeConfig, p.config());
    EXPECT_EQ(kFakeState, p.state());

    auto props = p.properties();
    auto iter = props.findProperty(PortPropertyEthernet::type());
    ASSERT_NE(props.end(), iter);
    EXPECT_EQ(sizeof(PortPropertyEthernet), iter->size());

    auto &eth = iter->property<PortPropertyEthernet>();
    EXPECT_EQ(kFakeCurr, eth.curr());
    EXPECT_EQ(kFakeAdvertised, eth.advertised());
    EXPECT_EQ(kFakeSupported, eth.supported());
    EXPECT_EQ(kFakePeer, eth.peer());
    EXPECT_EQ(0xAAAAAAAA, eth.currSpeed());
    EXPECT_EQ(0xBBBBBBBB, eth.maxSpeed());
  }
}

TEST(featuresreply, v2) {
  PortBuilder portBuilder;
  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(MacAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(kFakeConfig);
  portBuilder.setState(kFakeState);

  PortPropertyEthernet prop;
  prop.setCurr(kFakeCurr);
  prop.setAdvertised(kFakeAdvertised);
  prop.setSupported(kFakeSupported);
  prop.setPeer(kFakePeer);
  prop.setCurrSpeed(0xAAAAAAAA);
  prop.setMaxSpeed(0xBBBBBBBB);

  PropertyList properties;
  properties.add(prop);
  portBuilder.setProperties(properties);

  PortList ports;
  ports.add(portBuilder);
  ports.add(portBuilder);

  FeaturesReplyBuilder msg{1};
  msg.setDatapathId(DatapathID("2222222222222222"));
  msg.setBufferCount(0x33333333);
  msg.setTableCount(0x44);
  msg.setAuxiliaryId(0x55);
  msg.setCapabilities(kFakeCapabilities);
  msg.setActions(kFakeActions);
  msg.setPorts(ports);

  MemoryChannel channel{OFP_VERSION_2};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0xA0, channel.size());
  EXPECT_HEX(
      "020600A000000001222222222222222233333333440000006666666600000000"
      "11111111000000002222222222220000506F7274203300000000000000000000"
      "444444445555555566666666777777778888888899999999AAAAAAAABBBBBBBB"
      "11111111000000002222222222220000506F7274203300000000000000000000"
      "444444445555555566666666777777778888888899999999AAAAAAAABBBBBBBB",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.normalize();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  ASSERT_TRUE(reply);

  EXPECT_EQ(DatapathID("2222222222222222"), reply->datapathId());
  EXPECT_EQ(0x33333333, reply->bufferCount());
  EXPECT_EQ(0x44, reply->tableCount());
  EXPECT_EQ(0, reply->auxiliaryId());
  EXPECT_EQ(0x66666666, reply->capabilities());
  EXPECT_EQ(0, reply->actions());

  EXPECT_EQ(2, reply->ports().itemCount());

  for (auto &p : reply->ports()) {
    EXPECT_EQ(0x11111111, p.portNo());
    EXPECT_EQ(MacAddress{"22-22-22-22-22-22"}, p.hwAddr());
    EXPECT_EQ(SmallCString<16>("Port 3"), p.name());
    EXPECT_EQ(kFakeConfig, p.config());
    EXPECT_EQ(kFakeState, p.state());

    auto props = p.properties();
    auto iter = props.findProperty(PortPropertyEthernet::type());
    ASSERT_NE(props.end(), iter);
    EXPECT_EQ(sizeof(PortPropertyEthernet), iter->size());

    auto &eth = iter->property<PortPropertyEthernet>();
    EXPECT_EQ(kFakeCurr, eth.curr());
    EXPECT_EQ(kFakeAdvertised, eth.advertised());
    EXPECT_EQ(kFakeSupported, eth.supported());
    EXPECT_EQ(kFakePeer, eth.peer());
    EXPECT_EQ(0xAAAAAAAA, eth.currSpeed());
    EXPECT_EQ(0xBBBBBBBB, eth.maxSpeed());
  }
}

TEST(featuresreply, v1) {
  PortBuilder portBuilder;
  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(MacAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(kFakeConfig);
  portBuilder.setState(kFakeState);

  PortPropertyEthernet prop;
  prop.setCurr(kFakeCurr);
  prop.setAdvertised(kFakeAdvertised);
  prop.setSupported(kFakeSupported);
  prop.setPeer(kFakePeer);
  prop.setCurrSpeed(0xAAAAAAAA);
  prop.setMaxSpeed(0xBBBBBBBB);

  PropertyList properties;
  properties.add(prop);
  portBuilder.setProperties(properties);

  PortList ports;
  ports.add(portBuilder);
  ports.add(portBuilder);

  FeaturesReplyBuilder msg{1};
  msg.setDatapathId(DatapathID("2222222222222222"));
  msg.setBufferCount(0x33333333);
  msg.setTableCount(0x44);
  msg.setAuxiliaryId(0x55);
  msg.setCapabilities(kFakeCapabilities);
  msg.setActions(kFakeActions);
  msg.setPorts(ports);

  MemoryChannel channel{OFP_VERSION_1};
  UInt32 xid = msg.send(&channel);

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x80, channel.size());
  EXPECT_HEX(
      "01060080000000012222222222222222333333334400000066666666000006EF"
      "1111222222222222506F7274203300000000000000000000444444445555555566660666"
      "777707778888088899990999"
      "1111222222222222506F7274203300000000000000000000444444445555555566660666"
      "777707778888088899990999",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.normalize();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  ASSERT_TRUE(reply);

  EXPECT_EQ(DatapathID("2222222222222222"), reply->datapathId());
  EXPECT_EQ(0x33333333, reply->bufferCount());
  EXPECT_EQ(0x44, reply->tableCount());
  EXPECT_EQ(0, reply->auxiliaryId());
  EXPECT_EQ(0x66666666, reply->capabilities());
  EXPECT_EQ(0x40000677, reply->actions());

  EXPECT_EQ(2, reply->ports().itemCount());

  for (auto &p : reply->ports()) {
    EXPECT_EQ(0x1111, p.portNo());
    EXPECT_EQ(MacAddress{"22-22-22-22-22-22"}, p.hwAddr());
    EXPECT_EQ(SmallCString<16>("Port 3"), p.name());
    EXPECT_EQ(kFakeConfig, p.config());
    EXPECT_EQ(kFakeState, p.state());

    auto props = p.properties();
    auto iter = props.findProperty(PortPropertyEthernet::type());
    ASSERT_NE(props.end(), iter);
    EXPECT_EQ(sizeof(PortPropertyEthernet), iter->size());

    // N.B. loss of information
    auto &eth = iter->property<PortPropertyEthernet>();
    EXPECT_EQ(0x66666066, eth.curr());
    EXPECT_EQ(0x77777077, eth.advertised());
    EXPECT_EQ(0x88888808, eth.supported());
    EXPECT_EQ(0x99999819, eth.peer());
    EXPECT_EQ(0, eth.currSpeed());
    EXPECT_EQ(0, eth.maxSpeed());
  }
}
