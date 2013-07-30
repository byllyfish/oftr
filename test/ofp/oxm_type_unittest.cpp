#include <gtest/gtest.h>
#include "ofp/oxm_type.h"

using namespace ofp;


TEST(oxm_type, constructor)
{
	oxm_type a{2, 1, 32};
	EXPECT_EQ(0, std::memcmp(&a, "\0\2\2\4", 4));
	EXPECT_EQ(0x00020204UL, a.oxmNative());
	EXPECT_EQ(2, a.oxmClass());
	EXPECT_EQ(1, a.oxmField());
	EXPECT_EQ(4, a.length());
	EXPECT_FALSE(a.hasMask());

	oxm_type b{0x8000, 0xFF, 64};
	EXPECT_EQ(0, std::memcmp(&b, "\x80\x00\xFE\x08", 4));
	EXPECT_EQ(0x8000FE08UL, b.oxmNative());
	EXPECT_EQ(0x8000, b.oxmClass());
	EXPECT_EQ(0x7F, b.oxmField());
	EXPECT_EQ(8, b.length());
	EXPECT_FALSE(b.hasMask());
	
	oxm_type c{0xDEAD, 0x7F, 128};
	EXPECT_EQ(0, std::memcmp(&c, "\xDE\xAD\xFE\x10", 4));
	EXPECT_EQ(0xDEADFE10UL, c.oxmNative());
	EXPECT_EQ(0xDEAD, c.oxmClass());
	EXPECT_EQ(0x7F, c.oxmField());
	EXPECT_EQ(16, c.length());
	EXPECT_FALSE(c.hasMask());
}

TEST(oxm_type, use_in_switch_stmt)
{
	constexpr oxm_type cool{2, 2, 8};
	
	bool found = false;
	
	oxm_type d{2, 2, 8};
	switch (d)
	{
		case cool:
			found = true;
			break;
			
		case oxm_type{1, 1, 8}:
		case oxm_type{3, 3, 8}:
			break;
	}
	
	EXPECT_TRUE(found);
}

TEST(oxm_type, withMask)
{
	constexpr oxm_type a{2, 2, 8};
	EXPECT_FALSE(a.hasMask());
	
	constexpr oxm_type b = a.withMask();
	EXPECT_TRUE(b.hasMask());
	
	EXPECT_NE(a, b);
	EXPECT_EQ(a.oxmClass(), b.oxmClass());
	EXPECT_EQ(a.oxmField(), b.oxmField());
	EXPECT_EQ(a.length(), b.length());
	EXPECT_NE(a.hasMask(), b.hasMask());
	
	EXPECT_EQ(a.withMask(), b.withMask());
	
	constexpr oxm_type c = b.withoutMask();
	EXPECT_NE(b.hasMask(), c.hasMask());
	EXPECT_EQ(a, c);
	EXPECT_EQ(a.withoutMask(), c.withoutMask());
}
