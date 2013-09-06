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

TEST(bytelist, compare) 
{
	ByteList list1;
	ByteList list2;

	EXPECT_TRUE(list1 == list2);
	EXPECT_FALSE(list1 != list2);

	list1.add("foo", 3);
	EXPECT_FALSE(list1 == list2);
	EXPECT_TRUE(list1 != list2);

	list2.add("foo", 3);
	EXPECT_TRUE(list1 == list2);
	EXPECT_FALSE(list1 != list2);
}


TEST(bytelist, replace) 
{
	ByteList list;
	list.add("\x00\x11\x22", 3);

	EXPECT_EQ(3, list.size());
	EXPECT_HEX("00 11 22", list.data(), list.size());

	list.replace(list.data() + 1, list.data() + 3, "\x44\x55\x66\x77", 4);

	EXPECT_EQ(5, list.size());
	EXPECT_HEX("00 44 55 66 77", list.data(), list.size());
}

