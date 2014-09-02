#ifndef TESTAGENT_H
#define TESTAGENT_H

#include "ofp/ofp.h"
//#include "ofp/api/apiserver.h"

namespace testagent { // <namespace testagent>

using namespace ofp;

class TestAgent : public ChannelListener {
public:
  static TestAgent *Factory() { return new TestAgent; }

  void onChannelUp(Channel *channel) override;
  void onChannelDown(Channel *channel) override;
  void onMessage(const Message *message) override;
  void onTimer(UInt32 timerID) override;

private:
  Channel *channel_ = nullptr;
  //std::unique_ptr<api::ApiServer> apiServer_;

  void startApiInput();
};

} // </namespace testagent>

#endif // TESTAGENT_H
