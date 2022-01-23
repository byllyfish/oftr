// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/driver.h"

#include "ofp/unittest.h"

using namespace ofp;

class MockChannelListener : public ChannelListener {
 public:
  void onChannelUp(Channel *channel) override {}
  void onChannelDown(Channel *channel) override {}
  void onMessage(Message *message) override;
};

void MockChannelListener::onMessage(Message *message) {}

TEST(driver, test) {
  Driver driver;

  // Test installing signal handlers more than once.
  driver.installSignalHandlers();
  driver.installSignalHandlers();

  std::error_code err;
  UInt16 listenPort = UInt16_narrow_cast(OFPGetDefaultPort() + 10000);

  UInt64 connId = driver.listen(
      ChannelOptions::FEATURES_REQ, 0, {"127.0.0.1", listenPort},
      ProtocolVersions::All, [] { return new MockChannelListener; }, err);

  EXPECT_NE(0, connId);

  // Don't call driver.run(). This test just tests initialization.
  // driver.run();
}
