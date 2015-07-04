// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/smallcstring.h"

using namespace ofp;

TEST(smallcstring, empty) {
  SmallCString<1> s1;

  EXPECT_EQ(1, sizeof(s1));
  EXPECT_TRUE(IsStandardLayout<decltype(s1)>());
  EXPECT_TRUE(s1.empty());
  EXPECT_EQ(0, s1.length());
  EXPECT_EQ(1, s1.capacity());
  EXPECT_HEX("00", &s1, sizeof(s1));

  SmallCString<2> s2;

  EXPECT_EQ(2, sizeof(s2));
  EXPECT_TRUE(IsStandardLayout<decltype(s2)>());
  EXPECT_TRUE(s2.empty());
  EXPECT_EQ(0, s2.length());
  EXPECT_EQ(2, s2.capacity());
  EXPECT_HEX("0000", &s2, sizeof(s2));

  SmallCString<8> s8;

  EXPECT_EQ(8, sizeof(s8));
  EXPECT_TRUE(IsStandardLayout<decltype(s8)>());
  EXPECT_TRUE(s8.empty());
  EXPECT_EQ(0, s8.length());
  EXPECT_EQ(8, s8.capacity());
  EXPECT_HEX("0000 0000 0000 0000", &s8, sizeof(s8));

  SmallCString<9> s9;

  EXPECT_EQ(9, sizeof(s9));
  EXPECT_TRUE(IsStandardLayout<decltype(s9)>());
  EXPECT_TRUE(s9.empty());
  EXPECT_EQ(0, s9.length());
  EXPECT_EQ(9, s9.capacity());
  EXPECT_HEX("0000 0000 0000 0000 00", &s9, sizeof(s9));
}

TEST(smallcstring, constructor) {
  SmallCString<8> s{"test"};

  EXPECT_EQ(8, sizeof(s));
  EXPECT_FALSE(s.empty());
  EXPECT_EQ(4, s.length());
  EXPECT_HEX("74 65 73 74 00 00 00 00", &s, sizeof(s));
  EXPECT_EQ("test", s.toString());
}

TEST(smallcstring, copyconstructor) {
  const char *stuff = "stuffstuff";
  const SmallCString<8> *s = reinterpret_cast<const SmallCString<8> *>(stuff);

  EXPECT_EQ("stuffstu", s->toString());
  EXPECT_EQ(8, s->length());
  EXPECT_HEX("73 74 75 66 66 73 74 75", s, sizeof(*s));

  SmallCString<8> copy{*s};

  // We do not fix up the null termination when we make a copy.
  // SmallCString needs to have a trivial copy constructor.

  EXPECT_EQ(8, copy.length());
  EXPECT_HEX("73 74 75 66 66 73 74 75", &copy, sizeof(copy));

  SmallCString<8> copy2{"stuffstuff"};
  EXPECT_EQ(8, copy2.length());
  EXPECT_HEX("73 74 75 66 66 73 74 75", &copy2, sizeof(copy2));
}

TEST(smallcstring, validUtf8String) {
  auto validTest = [](const char *s) -> std::string {
    return ofp::detail::validUtf8String(s, s + strlen(s));
  };

  EXPECT_EQ("", validTest(""));
  EXPECT_EQ("a", validTest("a"));

  EXPECT_EQ(u8"\u00FF", validTest(u8"\u00FF"));
  EXPECT_EQ(u8"\u0080", validTest(u8"\u0080"));
  EXPECT_EQ(u8"\u80FF", validTest(u8"\u80FF"));
  EXPECT_EQ(u8"\uFFFF", validTest(u8"\uFFFF"));
  EXPECT_EQ(u8"\uFF00", validTest(u8"\uFF00"));
  EXPECT_EQ(u8"\u0FFF", validTest(u8"\u0FFF"));
  EXPECT_EQ(u8"\uFF00", validTest(u8"\uFF00"));
  EXPECT_EQ(u8"\U0010FFFF", validTest(u8"\U0010FFFF"));

  EXPECT_EQ("a?", validTest("a\xC2"));
  EXPECT_EQ("a?", validTest("a\x80"));

  EXPECT_EQ("??", validTest("\xC1\x9C"));
  EXPECT_EQ("\x41?\x3E\x42", validTest("\x41\xC2\x3E\x42"));

  EXPECT_EQ("????", validTest("\xF0\x82\x82\xAC"));
  EXPECT_EQ("??", validTest("\xC0\x80"));

  EXPECT_EQ("??????", validTest("\xED\xA1\x8C\xED\xBE\xB4"));
  EXPECT_EQ("????", validTest("\xF5\x80\x80\x80"));
  EXPECT_EQ("\xF4\x80\x80\x80", validTest("\xF4\x80\x80\x80"));
  EXPECT_EQ("\xF4\x8F\xBF\xBF", validTest("\xF4\x8F\xBF\xBF"));
  EXPECT_EQ("????", validTest("\xF4\x9F\xBF\xBF"));
  EXPECT_EQ("abc???", validTest("abc\xF4\x80\x80"));

  EXPECT_EQ("a?a", validTest(
                       "a\xFF"
                       "a"));
}
