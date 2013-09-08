#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp::yaml;

TEST(encoder, flowMod) 
{
	const char *input = "{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }";

	Encoder encoder;
	llvm::yaml::Input yin(input, nullptr, Encoder::diagnosticHandler, &encoder);

    yin >> encoder;
    const std::string &err = encoder.error();

    EXPECT_TRUE(yin.error());
    EXPECT_EQ("YAML:1:3: error: missing required key 'msg'\n{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }\n  ^\n", err);
}

TEST(encoder, hellov1) 
{
	const char *input = "{ type: OFPT_HELLO, version: 1, xid: 1 }";

	Encoder encoder;
	llvm::yaml::Input yin(input, nullptr, Encoder::diagnosticHandler, &encoder);
 
    yin >> encoder;
    const std::string &err = encoder.error();

    EXPECT_FALSE(yin.error());
    EXPECT_EQ("", err);

    EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
}


TEST(encoder, hellov4err) 
{
	const char *input = "{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, 4}} }";

	Encoder encoder;
	llvm::yaml::Input yin{input, nullptr, Encoder::diagnosticHandler, &encoder};

	EXPECT_TRUE(yin.error());
    EXPECT_EQ("YAML:1:43: error: Found unexpected ':' while scanning a plain scalar\n{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, 4}} }\n                                          ^\n", encoder.error());
}


TEST(encoder, hellov4) 
{
	const char *input = "{ 'type': 'OFPT_HELLO', 'version': 4, 'xid': 1, 'msg':{ 'versions': [1,4] } }";

	Encoder encoder;
	llvm::yaml::Input yin{input, nullptr, Encoder::diagnosticHandler, &encoder};

	EXPECT_FALSE(yin.error());

    yin >> encoder;

    EXPECT_FALSE(yin.error());
	EXPECT_EQ("", encoder.error());
    EXPECT_HEX("04000010000000010001000800000012", encoder.data(), encoder.size());
}

