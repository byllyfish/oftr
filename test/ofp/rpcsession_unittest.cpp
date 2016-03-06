// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcsession.h"
#include <future>
#include "ofp/unittest.h"

using namespace ofp;
using namespace ofp::rpc;

OFP_BEGIN_IGNORE_PADDING

class TestSession : public RpcSession {
 public:
  void receive(const std::string &msg) override {
    log::debug("TestSession::receive", msg);
    didReceive = true;
    stop();
  }

  bool didReceive = false;
};

OFP_END_IGNORE_PADDING

TEST(rpcsession, test) {
  TestSession session;

  session.send(
      R"""(
id: 1
method: foo
params: []
)""");

  EXPECT_FALSE(session.didReceive);

  auto f = std::async([&session]() { session.run(); });

  f.wait();

  EXPECT_TRUE(session.didReceive);
}
