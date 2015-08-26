// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/actionlist.h"
#include "ofp/actions.h"

using namespace ofp;

TEST(actionlist, empty) {
  ActionList list;

  EXPECT_EQ(0, list.size());
}

TEST(actionlist, one) {
  ActionList list;

  list.add(AT_COPY_TTL_OUT{});

  EXPECT_EQ(sizeof(AT_COPY_TTL_OUT), list.size());

  AT_COPY_TTL_OUT expected;
  EXPECT_EQ(0, std::memcmp(&expected, list.data(), list.size()));
}

TEST(actionlist, two) {
  ActionList list;

  list.add(AT_COPY_TTL_OUT{});
  list.add(AT_OUTPUT{255, 511});

  EXPECT_EQ(sizeof(AT_COPY_TTL_OUT) + sizeof(AT_OUTPUT), list.size());
}

TEST(actionlist, AT_SET_FIELD) {
  ByteRange data{"\xAA\xBB", 2};
  AT_SET_FIELD_CUSTOM act{X_EXPERIMENTER_01::type(),
                          X_EXPERIMENTER_01::experimenter(), data};

  ActionList list;
  list.add(act);
  list.add(AT_SET_FIELD<X_EXPERIMENTER_01>{0x1234});

  // FIXME(bfish): The second action in this test is not correct (There's no
  // experimenter value.)
  EXPECT_HEX("00190010FFFF060600FFFFFEAABB000000190010FFFF06061234000000000000",
             list.data(), list.size());
}
