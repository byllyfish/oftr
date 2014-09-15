#include "ofp/unittest.h"
#include "ofp/ofp.h"

using namespace ofp;

const UInt16 kTestingPort = 6666;

class TestController : public ChannelListener {
public:
  TestController() { ++controllerCount; }

  ~TestController() { --controllerCount; }

  void onChannelUp(Channel *channel) override {
    log::debug("TestController::onChannelUp");
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

  void onMessage(const Message *message) override {
    log::debug("TestController::onMessage");
    EXPECT_EQ(OFPT_FEATURES_REPLY, message->type());
  }

  static ChannelListener *factory() { return new TestController; }
  static int controllerCount;
  static int shutdownCount;

private:
  log::Lifetime lifetime_{"TestController"};
  Channel *channel_;
};

class TestAgent : public ChannelListener {
public:
  TestAgent() { ++agentCount; }

  ~TestAgent() { --agentCount; }

  void onChannelUp(Channel *channel) override {
    log::debug("TestAgent::onChannelUp");

    // When agent channel comes up, we expect the datapathId to be all zeros.
    DatapathID dpid;
    EXPECT_EQ(dpid, channel->datapathId());
    EXPECT_EQ(0, channel->auxiliaryId());
    EXPECT_LT(0, channel->version());
    EXPECT_GE(OFP_VERSION_LAST, channel->version());

    for (UInt8 i = 1; i <= auxCount; ++i) {
      channel->openAuxChannel(i, Channel::Transport::TCP);
    }
  }

  void onMessage(const Message *message) override {
    log::debug("TestAgent::onMessage");
    EXPECT_EQ(OFPT_FEATURES_REQUEST, message->type());

    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    FeaturesReplyBuilder reply{message->xid()};
    reply.setDatapathId(dpid);
    reply.send(message->source());
  }

  static ChannelListener *factory() { return new TestAgent; }
  static int agentCount;
  static int auxCount;

private:
  log::Lifetime lifetime_{"TestAgent"};
};

int TestController::controllerCount = 0;
int TestController::shutdownCount = 0;
int TestAgent::agentCount = 0;
int TestAgent::auxCount = 0;

TEST(roundtrip, basic_test) {
  // log::set(&std::cerr);

  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);

  {
    Driver driver;
    IPv6Address localhost{"127.0.0.1"};

    std::error_code err1;
    (void)driver.listen(ChannelMode::Controller,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions::All, TestController::factory, err1);
    EXPECT_FALSE(err1);

    auto result2 = driver.connect(ChannelMode::Agent,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions::All, TestAgent::factory);

    result2.done([](const std::error_code &err) { EXPECT_FALSE(err); });

    // The driver will run until the Controller shuts it down.
    driver.run();

    EXPECT_EQ(1, TestController::controllerCount);
    EXPECT_EQ(1, TestAgent::agentCount);
  }

  // Destruction is complete when Driver goes out of scope.
  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);
}

TEST(roundtrip, reconnect_test) {
  // log::set(&std::cerr);

  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);

  {
    Driver driver;
    IPv6Address localhost{"127.0.0.1"};

    std::error_code err1;
    (void)driver.listen(ChannelMode::Controller,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions::All, TestController::factory, err1);

    EXPECT_FALSE(err1);

    auto result2 = driver.connect(ChannelMode::Agent,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions::All, TestAgent::factory);

    result2.done([](const std::error_code &err) { EXPECT_FALSE(err); });

    TestController::shutdownCount = 3;
    driver.run();

    EXPECT_EQ(1, TestController::controllerCount);
    EXPECT_EQ(1, TestAgent::agentCount);
  }

  // Destruction is complete when Driver goes out of scope.
  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);
}

TEST(roundtrip, auxiliary_test) {
  // log::set(&std::cerr);

  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);

  {
    Driver driver;
    IPv6Address localhost{"127.0.0.1"};

    std::error_code err1;
    (void)driver.listen(ChannelMode::Controller,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions::All, TestController::factory, err1);

    EXPECT_FALSE(err1);

    auto result2 = driver.connect(ChannelMode::Agent,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions::All, TestAgent::factory);

    result2.done([](const std::error_code &err) { EXPECT_FALSE(err); });

    TestAgent::auxCount = 5;
    driver.run();

    EXPECT_EQ(1, TestController::controllerCount);
    EXPECT_EQ(1, TestAgent::agentCount);
  }

  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);
}
