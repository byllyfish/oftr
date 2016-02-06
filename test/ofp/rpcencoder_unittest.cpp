#include "ofp/unittest.h"
#include "ofp/rpc/rpcencoder.h"

using namespace ofp;

TEST(rpcencoder, test) {
    // N.B. we are passing a null RpcConnection. This will cause a crash if the
    // rpc call does not have an error. We are only testing *invalid* rpc calls.

    rpc::RpcEncoder encoder{R"""({"id":321,"method":"OFP.SEND","params":{"type":"foo"}})""", nullptr, nullptr};

    EXPECT_EQ("YAML:1:48: error: unknown value \"foo\" Did you mean \"HELLO\"?\n{\"id\":321,\"method\":\"OFP.SEND\",\"params\":{\"type\":\"foo\"}}\n                                               ^~~~~\n", encoder.error());
}
