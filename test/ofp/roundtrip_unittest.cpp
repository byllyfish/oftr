// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/ofp.h"

using namespace ofp;

const UInt16 kTestingPort = 6666;

class TestController : public ChannelListener {
public:
  TestController() { 
    log::debug("TestController constructed");
    ++controllerCount; 
  }

  ~TestController() { 
    log::debug("TestController destroyed");
    --controllerCount; 
  }

  void onChannelUp(Channel *channel) override {
    ++controllerCount; 
    log::debug("TestController::onChannelUp", std::make_pair("connid", channel->connectionId()));
    channel_ = channel;

    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    EXPECT_EQ(dpid, channel->datapathId());
    EXPECT_EQ(0, channel->auxiliaryId());

    channel->driver()->stop(1000_ms);

    if (shutdownCount > 0) {
      --shutdownCount;
      channel->shutdown();
    }
  }

  void onChannelDown(Channel *channel) override {
    --controllerCount; 
    log::debug("TestController::onChannelDown", std::make_pair("connid", channel->connectionId()));
  }

  void onMessage(const Message *message) override {
    log::debug("TestController::onMessage", std::make_pair("connid", message->source()->connectionId()));
    EXPECT_EQ(OFPT_FEATURES_REPLY, message->type());
  }

  static ChannelListener *factory() { return new TestController; }
  static int controllerCount;
  static int shutdownCount;

private:
  Channel *channel_;
};

class TestAgent : public ChannelListener {
public:
  TestAgent() { 
    ++agentCount; 
    log::debug("TestAgent constructed");
  }

  ~TestAgent() { 
    --agentCount; 
    log::debug("TestAgent destroyed");
  }

  void onChannelUp(Channel *channel) override {
    ++agentCount; 
    log::debug("TestAgent::onChannelUp", std::make_pair("connid", channel->connectionId()));

    // When agent channel comes up, we expect the datapathId to be all zeros.
    DatapathID dpid;
    EXPECT_EQ(dpid, channel->datapathId());
    EXPECT_EQ(0, channel->auxiliaryId());
    EXPECT_LT(0, channel->version());
    EXPECT_GE(OFP_VERSION_LAST, channel->version());
  }

  void onChannelDown(Channel *channel) override {
    --agentCount; 
    log::debug("TestAgent::onChannelDown", std::make_pair("connid", channel->connectionId()));
  }

  void onMessage(const Message *message) override {
    log::debug("TestAgent::onMessage", std::make_pair("connid", message->source()->connectionId()));
    EXPECT_EQ(OFPT_FEATURES_REQUEST, message->type());

    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    FeaturesReplyBuilder reply{message->xid()};
    reply.setDatapathId(dpid);
    reply.send(message->source());
  }

  static ChannelListener *factory() { return new TestAgent; }
  static int agentCount;
  static int auxCount;
};

int TestController::controllerCount = 0;
int TestController::shutdownCount = 0;
int TestAgent::agentCount = 0;
int TestAgent::auxCount = 0;

TEST(roundtrip, basic_test) {

  // Before we start, there are no controller or agent listeners.
  TestController::controllerCount = 0;
  TestAgent::agentCount = 0;

  {
    Driver driver;
    IPv6Address localhost{"127.0.0.1"};

    std::error_code err1;
    (void)driver.listen(ChannelMode::Controller, 0,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions::All, TestController::factory, err1);
    // There should be no error on listen, unless the port is in use.
    EXPECT_FALSE(err1);

    (void)driver.connect(ChannelMode::Raw, 0,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions::All, TestAgent::factory, [](Channel *, std::error_code err){
                                    EXPECT_FALSE(err);
                                  });

    // The driver will run until the Controller shuts it down.
    driver.run();

    EXPECT_EQ(2, TestController::controllerCount);
    EXPECT_EQ(2, TestAgent::agentCount);
  }

  // Destruction is complete when Driver goes out of scope.
  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);
}

TEST(roundtrip, shutdown_test) {

  TestController::controllerCount = 0;
  TestAgent::agentCount = 0;

  {
    Driver driver;
    IPv6Address localhost{"127.0.0.1"};

    std::error_code err1;
    (void)driver.listen(ChannelMode::Controller, 0,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions::All, TestController::factory, err1);

    EXPECT_FALSE(err1);

    (void)driver.connect(ChannelMode::Raw, 0,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions::All, TestAgent::factory, [](Channel *, std::error_code err){
                                    EXPECT_FALSE(err);
                                  });

    TestController::shutdownCount = 1;
    driver.run();

    EXPECT_EQ(0, TestController::shutdownCount);
    EXPECT_EQ(0, TestController::controllerCount);
    EXPECT_EQ(0, TestAgent::agentCount);
  }

  // Destruction is complete when Driver goes out of scope.
  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);
}

