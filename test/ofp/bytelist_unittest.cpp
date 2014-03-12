//  ===== ---- ofp/bytelist_unittest.cpp -------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements unit tests for ByteList class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/bytelist.h"

using namespace ofp;

TEST(bytelist, basic) {
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

TEST(bytelist, uninitialized) {
  ByteList list;

  list.addUninitialized(25);
  EXPECT_EQ(25, list.size());
}

TEST(bytelist, copy) {
  ByteList list;

  list.add("abc", 3);
  EXPECT_EQ(3, list.size());
  ByteList list2{list};
  EXPECT_EQ(3, list2.size());
}

TEST(bytelist, compare) {
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

TEST(bytelist, replace) {
  ByteList list;
  list.add("\x00\x11\x22", 3);

  EXPECT_EQ(3, list.size());
  EXPECT_HEX("00 11 22", list.data(), list.size());

  list.replace(list.data() + 1, list.data() + 3, "\x44\x55\x66\x77", 4);

  EXPECT_EQ(5, list.size());
  EXPECT_HEX("00 44 55 66 77", list.data(), list.size());
}

TEST(bytelist, byterange) {
  ByteList list;
  list.add("123", 3);

  ByteRange range = list.toRange();

  ByteList list2{range};
  ByteList list3;
  list3 = range;

  EXPECT_EQ(list2, list3);
}

TEST(bytelist, move) {
  ByteList list;
  list.add("123", 3);

  ByteList list2;
  list2.add("x", 1);

  EXPECT_EQ(list.size(), 3);
  list2 = std::move(list);
  EXPECT_EQ(0, list.size());
}
