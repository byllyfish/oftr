#include "ofp/unittest.h"
#include "ofp/yaml/encoder.h"

using namespace ofp::yaml;

TEST(encoder, flowMod) 
{
	const char *input = "{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }";

	Encoder encoder{input};
    const std::string &err = encoder.error();

    EXPECT_EQ("YAML:1:3: error: missing required key 'msg'\n{ type: OFPT_FLOW_MOD, version: 4, xid: 1 }\n  ^\n", err);
}

TEST(encoder, hellov1) 
{
	const char *input = "{ type: OFPT_HELLO, version: 1, xid: 1 }";

	Encoder encoder{input};
    const std::string &err = encoder.error();

    EXPECT_EQ("", err);
    EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
}


TEST(encoder, hellov4err) 
{
	const char *input = "{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, 4}} }";

	Encoder encoder{input};

    EXPECT_EQ("YAML:1:43: error: Found unexpected ':' while scanning a plain scalar\n{ type: OFPT_HELLO, version: 4, xid: 1 msg:{versions{1, 4}} }\n                                          ^\n", encoder.error());
}


TEST(encoder, hellov4) 
{
	const char *input = "{ 'type': 'OFPT_HELLO', 'version': 4, 'xid': 99, 'msg':{ 'versions': [1,4] } }";

	Encoder encoder{input};

	EXPECT_EQ("", encoder.error());
    EXPECT_HEX("04000010000000630001000800000012", encoder.data(), encoder.size());
}


TEST(encoder, error) 
{
	const char *input = R"""(
type: OFPT_ERROR
version: 1
xid: 98
msg:
  type: 1
  code: 1
  data: 'FFFF1234567890'
)""";

	Encoder encoder{input};

	EXPECT_EQ("", encoder.error());
    EXPECT_HEX("0101000C0000006200010001FFFF1234567890", encoder.data(), encoder.size());
}

TEST(encoder, error2) 
{
	const char *input = R"""(
type: OFPT_ERROR
version: 1
xid: 98
msg:
  type: 1
  code: 1
  data: 'Not Hex'
)""";

	Encoder encoder{input};

	EXPECT_EQ("YAML:8:9: error: Invalid hexadecimal text.\n  data: 'Not Hex'\n        ^~~~~~~~~\n", encoder.error());
    EXPECT_HEX("0101000C0000006200010001", encoder.data(), encoder.size());
}

