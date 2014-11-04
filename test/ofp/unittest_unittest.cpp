// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"

using namespace std;

TEST(unittest, test) {
  string a = hexclean("00 01 \n - 02 z$ 03");
  EXPECT_EQ("00010203", a);

  char d[] = {1, 2, 3, 4};
  EXPECT_HEX("01020304", d, sizeof(d));
}
