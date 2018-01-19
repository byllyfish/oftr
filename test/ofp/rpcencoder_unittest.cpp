// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/rpcencoder.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(rpcencoder, ofp_send_invalid_type) {
  // N.B. we are passing a null RpcConnection. This will cause a crash if the
  // rpc call does not have an error. We are only testing *invalid* rpc calls.

  rpc::RpcEncoder encoder{
      R"""({"id":321,"method":"OFP.SEND","params":{"type":"foo"}})""", nullptr,
      nullptr};

  EXPECT_EQ(
      "YAML:1:48: error: unknown value \"foo\" Did you mean "
      "\"HELLO\"?\n{\"id\":321,\"method\":\"OFP.SEND\",\"params\":{\"type\":"
      "\"foo\"}}\n                                               ^~~~~\n",
      encoder.error());
}

TEST(rpcencoder, end_brace_only) {
  rpc::RpcEncoder encoder{"  }", nullptr, nullptr};

  EXPECT_EQ("YAML:1:3: error: Unexpected token\n  }\n  ^\n", encoder.error());
}
