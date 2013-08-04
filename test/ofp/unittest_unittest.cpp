#include "ofp/unittest.h"

using namespace std;


TEST(unittest, test) 
{
	string s = HEX("00 01 02 03");
	// s is byte rep of hex.

	string a = HEXCLEAN("00 01 \n - 02 z$ 03");
	EXPECT_EQ("00010203", a);

	int c = 0x01020304;
	//HEXDUMP("c =", c);

	std::cout << TOHEX(c) << '\n';

	char d[] = { 1, 2, 3, 4 };
	EXPECT_HEX("01020304", d, sizeof(d));
}
