// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/featuresreply.h"

using namespace ofp;

const OFPActionTypeFlags kFakeActions = static_cast<OFPActionTypeFlags>(0x77777777);
const OFPCapabilitiesFlags kFakeCapabilities = static_cast<OFPCapabilitiesFlags>(0x66666666);

TEST(featuresreply, v4) {
  PortBuilder portBuilder;
  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(EnetAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(0x44444444);
  portBuilder.setState(0x55555555);
  portBuilder.setCurr(0x66666666);
  portBuilder.setAdvertised(0x77777777);
  portBuilder.setSupported(0x88888888);
  portBuilder.setPeer(0x99999999);
  portBuilder.setCurrSpeed(0xAAAAAAAA);
  portBuilder.setMaxSpeed(0xBBBBBBBB);

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

  EXPECT_EQ(1, xid);
  EXPECT_EQ(0x20, channel.size());
  EXPECT_HEX("0406002000000001222222222222222233333333445500006666666600000000",
             channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  EXPECT_TRUE(reply);

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
  portBuilder.setHwAddr(EnetAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(0x44444444);
  portBuilder.setState(0x55555555);
  portBuilder.setCurr(0x66666666);
  portBuilder.setAdvertised(0x77777777);
  portBuilder.setSupported(0x88888888);
  portBuilder.setPeer(0x99999999);
  portBuilder.setCurrSpeed(0xAAAAAAAA);
  portBuilder.setMaxSpeed(0xBBBBBBBB);

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
  message.transmogrify();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  EXPECT_TRUE(reply);

  EXPECT_EQ(DatapathID("2222222222222222"), reply->datapathId());
  EXPECT_EQ(0x33333333, reply->bufferCount());
  EXPECT_EQ(0x44, reply->tableCount());
  EXPECT_EQ(0, reply->auxiliaryId());
  EXPECT_EQ(0x66666666, reply->capabilities());
  EXPECT_EQ(0, reply->actions());

  EXPECT_EQ(2 * sizeof(Port), reply->ports().size());
}

TEST(featuresreply, v2) {
  PortBuilder portBuilder;
  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(EnetAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(0x44444444);
  portBuilder.setState(0x55555555);
  portBuilder.setCurr(0x66666666);
  portBuilder.setAdvertised(0x77777777);
  portBuilder.setSupported(0x88888888);
  portBuilder.setPeer(0x99999999);
  portBuilder.setCurrSpeed(0xAAAAAAAA);
  portBuilder.setMaxSpeed(0xBBBBBBBB);

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
  message.transmogrify();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  EXPECT_TRUE(reply);

  EXPECT_EQ(DatapathID("2222222222222222"), reply->datapathId());
  EXPECT_EQ(0x33333333, reply->bufferCount());
  EXPECT_EQ(0x44, reply->tableCount());
  EXPECT_EQ(0, reply->auxiliaryId());
  EXPECT_EQ(0x66666666, reply->capabilities());
  EXPECT_EQ(0, reply->actions());

  EXPECT_EQ(2 * sizeof(Port), reply->ports().size());
}

TEST(featuresreply, v1) {
  PortBuilder portBuilder;
  portBuilder.setPortNo(0x11111111);
  portBuilder.setHwAddr(EnetAddress{"22-22-22-22-22-22"});
  portBuilder.setName("Port 3");
  portBuilder.setConfig(0x44444444);
  portBuilder.setState(0x55555555);
  portBuilder.setCurr(0x66666666);
  portBuilder.setAdvertised(0x77777777);
  portBuilder.setSupported(0x88888888);
  portBuilder.setPeer(0x99999999);
  portBuilder.setCurrSpeed(0xAAAAAAAA);
  portBuilder.setMaxSpeed(0xBBBBBBBB);

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
      "1111222222222222506F7274203300000000000000000000444444445555555566666666"
      "777777778888888899999999"
      "1111222222222222506F7274203300000000000000000000444444445555555566666666"
      "777777778888888899999999",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const FeaturesReply *reply = FeaturesReply::cast(&message);
  ASSERT_TRUE(reply);

  EXPECT_EQ(DatapathID("2222222222222222"), reply->datapathId());
  EXPECT_EQ(0x33333333, reply->bufferCount());
  EXPECT_EQ(0x44, reply->tableCount());
  EXPECT_EQ(0, reply->auxiliaryId());
  EXPECT_EQ(0x66666666, reply->capabilities());
  EXPECT_EQ(0x40000677, reply->actions());

  EXPECT_EQ(2 * sizeof(Port), reply->ports().size());
}
