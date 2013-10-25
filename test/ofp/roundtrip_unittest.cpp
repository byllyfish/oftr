#include "ofp/unittest.h"
#include "ofp.h"

using namespace ofp;

const UInt16 kTestingPort = 6666;

class TestController : public ChannelListener {
public:
  TestController() { ++controllerCount; }

  ~TestController() { --controllerCount; }

  void onChannelUp(Channel *channel) override {
    channel_ = channel;

    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    EXPECT_EQ(dpid, channel->datapathId());
    EXPECT_EQ(0, channel->auxiliaryId());

    channel->scheduleTimer(0x5678, 1000_ms);
    if (shutdownCount > 0) {
      --shutdownCount;
      channel->shutdown();
    }
  }

  void onMessage(const Message *message) override {
    EXPECT_TRUE(false); // never called
  }

  void onException(const Exception *exception) override {
    EXPECT_TRUE(false); // never called
  }

  void onTimer(UInt32 timerID) {
    EXPECT_EQ(0x5678, timerID);

    channel_->driver()->stop();
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
    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    EXPECT_EQ(channel->datapathId(), dpid);
    EXPECT_EQ(0, channel->auxiliaryId());

    for (UInt8 i = 1; i <= auxCount; ++i) {
      channel->openAuxChannel(i, Channel::Transport::TCP);
    }
  }

  void onMessage(const Message *message) override {}

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

    auto result1 = driver.listen(Driver::Controller, nullptr,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions{}, TestController::factory);

    result1.done([](Exception exc) { EXPECT_FALSE(exc); });

    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    Features features{dpid};

    auto result2 = driver.connect(Driver::Agent, &features,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions{}, TestAgent::factory);

    result2.done([](Exception exc) { EXPECT_FALSE(exc); });

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

    auto result1 = driver.listen(Driver::Controller, nullptr,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions{}, TestController::factory);

    result1.done([](Exception exc) { EXPECT_FALSE(exc); });

    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    Features features{dpid};

    auto result2 = driver.connect(Driver::Agent, &features,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions{}, TestAgent::factory);

    result2.done([](Exception exc) { EXPECT_FALSE(exc); });

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

    auto result1 = driver.listen(Driver::Controller, nullptr,
                                 IPv6Endpoint{localhost, kTestingPort},
                                 ProtocolVersions{}, TestController::factory);

    result1.done([](Exception exc) { EXPECT_FALSE(exc); });

    DatapathID dpid{0x1234, EnetAddress{"A1:B2:C3:D4:E5:F6"}};
    Features features{dpid};

    auto result2 = driver.connect(Driver::Agent, &features,
                                  IPv6Endpoint{localhost, kTestingPort},
                                  ProtocolVersions{}, TestAgent::factory);

    result2.done([](Exception exc) { EXPECT_FALSE(exc); });

    TestAgent::auxCount = 5;
    driver.run();

    EXPECT_EQ(1, TestController::controllerCount);
    EXPECT_EQ(1, TestAgent::agentCount);
  }

  EXPECT_EQ(0, TestController::controllerCount);
  EXPECT_EQ(0, TestAgent::agentCount);
}
