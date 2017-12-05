// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
  void onMessage(Message *message) override;

 private:
  void writeMessages(Channel *channel);
};

}  // namespace testagent

#endif  // EXAMPLE_TESTAGENT_TESTAGENT_H_
