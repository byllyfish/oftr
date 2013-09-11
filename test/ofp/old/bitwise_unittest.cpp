#include "ofp/unittest.h"
#include "ofp/bitwise.h"

using namespace ofp;

enum class Flags : UInt32 {
	a = 1 << 0,
	b = 1 << 1,
	c = 1 << 2
};

OFP_DEFINE_BITWISE_OPERATORS(Flags)


TEST(bitwise, test) 
{
	UInt32 a = +Flags::a;
	EXPECT_EQ(1, a);

	UInt32 b = Flags::a | Flags::b;
	EXPECT_EQ(3, b);

	UInt32 c = ~Flags::c;
	EXPECT_EQ(~4UL, c);
}
