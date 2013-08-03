#include <gtest/gtest.h>
#include "ofp/instructions.h"
#include "ofp/actions.h"
#include "ofp/actionlist.h"

using namespace ofp;

static const char *HEX(const char *data) { return HexToRawData(data).data(); }


TEST(instructions, IT_GOTO_TABLE) {
	IT_GOTO_TABLE gt{5};

	EXPECT_EQ(5, gt.table());
	EXPECT_EQ(8, sizeof(gt));

	auto expected = HexToRawData("0001 0008 05 000000");
	EXPECT_EQ(0, std::memcmp(expected.data(), &gt, sizeof(gt)));
}

TEST(instructions, IT_WRITE_METADATA) {
	IT_WRITE_METADATA wm{1, 2};

	EXPECT_EQ(1, wm.metadata());
	EXPECT_EQ(2, wm.mask());
	EXPECT_EQ(24, sizeof(wm));

	auto expected = HexToRawData("0002 0018 0000 0000 0000 0000 0000 0001 0000 0000 0000 0002");
	EXPECT_EQ(0, std::memcmp(expected.data(), &wm, sizeof(wm)));
}

TEST(instructions, IT_WRITE_ACTIONS) {
	ActionList list;
	list.add(AT_COPY_TTL_OUT{});

	IT_WRITE_ACTIONS wa{&list};
	EXPECT_EQ(8, IT_WRITE_ACTIONS::HeaderSize);
	EXPECT_EQ(0, std::memcmp(HEX("0003 0010 0000 0000"), &wa, IT_WRITE_ACTIONS::HeaderSize));
	EXPECT_EQ(0, std::memcmp(HEX("000B 0008 0000 0000"), wa.data(), wa.size()));
}