#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp::yaml;

TEST(encoder, flowMod) 
{
	const char *input = "{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }";

	llvm::yaml::Input yin(input);
    Encoder encoder;
    yin.setDiagHandler(Encoder::diagnosticHandler, &encoder);

    yin >> encoder;
    const std::string &err = encoder.error();

    EXPECT_TRUE(yin.error());
    EXPECT_EQ("YAML:1:3: error: missing required key 'msg'\n{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }\n  ^\n", err);
}

TEST(encoder, hellov1) 
{
	const char *input = "{ type: OFPT_HELLO, version: 1, xid: 1 }";

	llvm::yaml::Input yin(input);
    Encoder encoder;
    yin.setDiagHandler(Encoder::diagnosticHandler, &encoder);

    yin >> encoder;
    const std::string &err = encoder.error();

    EXPECT_FALSE(yin.error());
    EXPECT_EQ("", err);

    EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
}

#if 0
TEST(encoder, hellov4) 
{
	const char *input = "{ type: OFPT_HELLO, version: 1, xid: 1 msg:{versions{1, 4}} }";

	llvm::yaml::Input yin(input);
	EXPECT_FALSE(yin.error());

    Encoder encoder;
    yin.setDiagHandler(Encoder::diagnosticHandler, &encoder);
    if (!yin.error()) {
    	yin >> encoder;
    	const std::string &err = encoder.error();

    	EXPECT_FALSE(yin.error());
    	EXPECT_EQ("", err);

    	EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
    }
}
#endif
