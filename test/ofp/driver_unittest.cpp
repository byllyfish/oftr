// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/driver.h"

using namespace ofp;

class MockChannelListener : public ChannelListener {
 public:
  void onChannelUp(Channel *channel) override {}
  void onChannelDown(Channel *channel) override {}
  void onMessage(const Message *message) override;
};

void MockChannelListener::onMessage(const Message *message) {
}

TEST(driver, test) {
  Driver driver;
  std::error_code err;

  UInt64 connId = driver.listen(
      ChannelMode::Controller, 0, IPv6Endpoint{OFP_DEFAULT_PORT},
      ProtocolVersions::All, [] { return new MockChannelListener; }, err);

  EXPECT_NE(0, connId);
  // driver.run();
}
