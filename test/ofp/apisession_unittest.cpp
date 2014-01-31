#include "ofp/unittest.h"
#include "ofp/api/apisession.h"
#include <future>

using namespace ofp;
using namespace ofp::api;

OFP_BEGIN_IGNORE_PADDING

class TestSession : public ApiSession {
public:
  void receive(const std::string &msg) override {
    log::debug("TestSession::receive", msg);
    didReceive = true;
    stop();
  }

  bool didReceive = false;
};

OFP_END_IGNORE_PADDING


TEST(apisession, test) {
  TestSession session;

  session.send(
R"""(
---
foo: foo
...
)""");

  EXPECT_FALSE(session.didReceive);

  auto f = std::async([&session]() { 
    session.run(); 
  });

  f.wait();

  EXPECT_TRUE(session.didReceive);
}
