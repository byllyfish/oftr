#include "ofp/unittest.h"
#include "ofp/bytelist.h"

using namespace ofp;


TEST(bytelist, basic) 
{
	ByteList list;

	EXPECT_EQ(0, list.size());
	EXPECT_EQ(nullptr, list.begin());
	EXPECT_EQ(nullptr, list.end());

	list.add("a", 1);
	EXPECT_EQ(1, list.size());
	EXPECT_NE(nullptr, list.begin());
	EXPECT_NE(nullptr, list.end());
	EXPECT_EQ('a', list[0]);

	int cnt = 0;
	for (auto i : list) {
		EXPECT_EQ('a', i);
		++cnt;
	}
	EXPECT_EQ(1, cnt);

	list.clear();
	EXPECT_EQ(0, list.size());

	// Should not release its memory when cleared.
	EXPECT_NE(nullptr, list.begin());
	EXPECT_NE(nullptr, list.end());
}


TEST(bytelist, uninitialized) 
{
	ByteList list;

	list.addUninitialized(25);
	EXPECT_EQ(25, list.size());
}


TEST(bytelist, copy)
{
	ByteList list;

	list.add("abc", 3);
	EXPECT_EQ(3, list.size());
	ByteList list2{list};
	EXPECT_EQ(3, list2.size());
}
