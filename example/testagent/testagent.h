#ifndef TESTAGENT_H
#define TESTAGENT_H

#include "ofp/ofp.h"

namespace testagent { // <namespace testagent>

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

} // </namespace testagent>

#endif // TESTAGENT_H
