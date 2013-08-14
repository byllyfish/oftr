#include "ofp/unittest.h"
#include "ofp/actiontype.h"


using namespace ofp;


TEST(actiontype, AT_OUTPUT)
{
	ActionType type{ActionType::AT_OUTPUT, 8};

	EXPECT_EQ(4, sizeof(type));
	EXPECT_EQ(ActionType::AT_OUTPUT, type.type());
	EXPECT_EQ(8, type.length());
	EXPECT_EQ(0x08, type.nativeType());

	auto expected = HexToRawData("00000008");
	EXPECT_EQ(0, std::memcmp(expected.data(), &type, sizeof(type)));
}


TEST(actiontype, AT_EXPERIMENTER)
{
	ActionType type{ActionType::AT_EXPERIMENTER, 64};

	EXPECT_EQ(4, sizeof(type));
	EXPECT_EQ(ActionType::AT_EXPERIMENTER, type.type());
	EXPECT_EQ(64, type.length());
	EXPECT_EQ(0xFFFF0040, type.nativeType());
	
	auto expected = HexToRawData("FFFF0040");
	EXPECT_EQ(0, std::memcmp(expected.data(), &type, sizeof(type)));
}

TEST(actiontype, AT_EXPERIMENTER_2)
{
	ActionType type{ActionType::AT_EXPERIMENTER, 65530};

	EXPECT_EQ(4, sizeof(type));
	EXPECT_EQ(ActionType::AT_EXPERIMENTER, type.type());
	EXPECT_EQ(65530, type.length());
	EXPECT_EQ(0xFFFFFFFA, type.nativeType());
	
	auto expected = HexToRawData("FFFFFFFA");
	EXPECT_EQ(0, std::memcmp(expected.data(), &type, sizeof(type)));
}

