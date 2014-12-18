// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef EXAMPLE_TESTAGENT_TESTAGENT_H_
#define EXAMPLE_TESTAGENT_TESTAGENT_H_

#include "ofp/ofp.h"

namespace testagent {

using namespace ofp;

class TestAgent : public ChannelListener {
 public:
  static TestAgent *Factory() { return new TestAgent; }

  void onChannelUp(Channel *channel) override {}
  void onChannelDown(Channel *channel) override {}
  void onMessage(const Message *message) override;

 private:
  void writeMessages(Channel *channel);
};

}  // namespace testagent

#endif  // EXAMPLE_TESTAGENT_TESTAGENT_H_
