// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/actionlist.h"
#include "ofp/actions.h"
#include "ofp/nicira.h"
#include "ofp/unittest.h"

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
  ByteRange data{"\x12\x34", 2};
  AT_SET_FIELD_CUSTOM act{X_EXPERIMENTER_01::type(),
                          X_EXPERIMENTER_01::experimenter(), data};

  ActionList list;
  list.add(act);
  list.add(AT_SET_FIELD<X_EXPERIMENTER_01>{0x1234});

  // FIXME(bfish): The second action in this test is not correct (There's no
  // experimenter value.)
  // TODO(bfish): Add a template specialization for AT_SET_FIELD<TYPE> where
  // TYPE has a experimenter() static field. Both actions should be identical.
  EXPECT_HEX("00190010FFFF060600FFFFFE1234000000190010FFFF06061234000000000000",
             list.data(), list.size());
}

TEST(actionlist, NX_AT_REGMOVE) {
  OXMRegister src{"IPV4_SRC[0:32]"};
  OXMRegister dst{"IPV4_DST[0:32]"};

  ActionList list;
  list.add(nx::AT_REGMOVE{src, dst});

  EXPECT_HEX("FFFF00180000232000060020000000008000160480001804", list.data(),
             list.size());
}
