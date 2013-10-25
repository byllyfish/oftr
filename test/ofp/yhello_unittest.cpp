#include "ofp/unittest.h"
#include "ofp/yaml/yhello.h"
#include "ofp/yaml/encoder.h"

using namespace ofp::yaml;

// N.B. Hello is the only protocol message where the version in the header can
// be overridden by something later in the payload.

TEST(yhello, test1) 
{
	// Standard version 4 works.
	const char *yaml = 
R"(
type: OFPT_HELLO
version: 4
msg: 
  versions: [1, 2, 3, 4]
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0400001000000000000100080000001E", encoder.data(), encoder.size());
}


TEST(yhello, test2) 
{
	// The protocol version is still version 4 (use highest protocol in versions). 
	const char *yaml = 
R"(
type: OFPT_HELLO
version: 1
msg: 
  versions: [1, 2, 3, 4]
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0400001000000000000100080000001E", encoder.data(), encoder.size());
}


TEST(yhello, test3) 
{
	// Always use higest protocol in versions rather than header version.
	const char *yaml = 
R"(
type: OFPT_HELLO
version: 4
msg: 
  versions: [1, 2]
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0200000800000000", encoder.data(), encoder.size());
}

TEST(yhello, test4) 
{
	// Empty version list means use version from header.
	const char *yaml = 
R"(
type: OFPT_HELLO
version: 1
msg:
  versions: []
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0100000800000000", encoder.data(), encoder.size());
}


TEST(yhello, test5) 
{
	// Omitted version list means use version from header.
	const char *yaml = 
R"(
type: OFPT_HELLO
version: 3
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0300000800000000", encoder.data(), encoder.size());
}

TEST(yhello, test6) 
{
	// No version in header? Specify all versions.
	const char *yaml = 
R"(
type: OFPT_HELLO
msg:
  versions: []
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0400001000000000000100080000001E", encoder.data(), encoder.size());
}

TEST(yhello, test7) 
{
	// No version in header? Specify all versions.
	const char *yaml = 
R"(
type: OFPT_HELLO
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0400001000000000000100080000001E", encoder.data(), encoder.size());
}

TEST(yhello, test8) 
{
	const char *yaml = 
R"(
type: OFPT_HELLO
msg:
  versions: [0, 1, 3, 5, 99]
)";

	Encoder encoder{yaml};
	EXPECT_EQ("", encoder.error());
	EXPECT_HEX("0300000800000000", encoder.data(), encoder.size());
}


TEST(yhello, error1) 
{
	const char *yaml = 
R"(
type: OFPT_HELLO
msg:
  version: [1, 2, 3, 4]
)";

	Encoder encoder{yaml};
	EXPECT_TRUE(encoder.error().find("missing required key 'versions'") != std::string::npos);
	EXPECT_HEX("", encoder.data(), encoder.size());
}


TEST(yhello, error2) 
{
	const char *yaml = 
R"(
type: OFPT_HELLO
msg:
)";

	Encoder encoder{yaml};
	EXPECT_TRUE(encoder.error().find("not a mapping") != std::string::npos);
	EXPECT_HEX("", encoder.data(), encoder.size());
}

