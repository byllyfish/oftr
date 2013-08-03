#include "ofp/unittest.h"

using namespace std;


TEST(unittest, test) 
{
	string s = HEX("00 01 02 03");
	// s is byte rep of hex.

	string a = HEXSTR("00 01 \n - 02 z$ 03");
	EXPECT_EQ("00010203", a);

	int c = 0x01020304;
	HEXDUMP("c =", c);
}
