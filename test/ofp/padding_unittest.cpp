#include <gtest/gtest.h>
#include "ofp/padding.h"

using namespace ofp;


TEST(padding, PadLength)
{
	EXPECT_EQ(0, PadLength(0));
	EXPECT_EQ(8, PadLength(8));
	for (unsigned i = 1; i < 8; ++i)
		EXPECT_EQ(8, PadLength(i));
	
	for (unsigned i = 0; i < 100; ++i) {
		EXPECT_TRUE(PadLength(i) % 8 == 0);
		EXPECT_TRUE(PadLength(i) >= i);
		EXPECT_TRUE(PadLength(i) - i <= 7);
	}
}


TEST(padding, Padding)
{
	{
		Padding<1>	pad;
		EXPECT_EQ(1, sizeof(pad));
	}
	
	{
		Padding<2>	pad;
		EXPECT_EQ(2, sizeof(pad));
	}

	{
		Padding<7>	pad;
		EXPECT_EQ(7, sizeof(pad));
	}
	
	{
		Padding<8>	pad;
		EXPECT_EQ(8, sizeof(pad));
	}
	
	{
		Padding<9>	pad;
		EXPECT_EQ(9, sizeof(pad));
	}
}
