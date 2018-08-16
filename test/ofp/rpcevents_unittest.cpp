#include "ofp/rpc/rpcevents.h"
#include "ofp/unittest.h"

TEST(rpcevents, test_TrimErrorMessage) {
  std::string msg1 = "x\n     ^";
  ofp::rpc::TrimErrorMessage(msg1);
  EXPECT_EQ(msg1, "x");

  std::string msg2 = "";
  ofp::rpc::TrimErrorMessage(msg2);
  EXPECT_EQ(msg2, "");

  std::string msg3 = "abc";
  ofp::rpc::TrimErrorMessage(msg3);
  EXPECT_EQ(msg3, "abc");
}
