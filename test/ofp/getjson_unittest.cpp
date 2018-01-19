// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/getjson.h"
#include "ofp/unittest.h"

using namespace ofp::yaml;

TEST(getjson, testOne) {
  std::istringstream iss{" { a } "};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{ a }", msg);
  EXPECT_EQ(1, lineNum);
  EXPECT_EQ(0, newlines);

  EXPECT_FALSE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("", msg);
  EXPECT_EQ(-1, lineNum);
  EXPECT_EQ(0, newlines);
}

TEST(getjson, testMultiple) {
  std::istringstream iss{"ignore {a}\n{{b}}\n{{{c}}}\n []"};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{a}", msg);
  EXPECT_EQ(1, lineNum);
  EXPECT_EQ(0, newlines);

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{{b}}", msg);
  EXPECT_EQ(2, lineNum);
  EXPECT_EQ(1, newlines);

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{{{c}}}", msg);
  EXPECT_EQ(3, lineNum);
  EXPECT_EQ(2, newlines);

  EXPECT_FALSE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("", msg);
  EXPECT_EQ(3, newlines);
  EXPECT_EQ(-1, lineNum);
}

TEST(getjson, testEmpty) {
  std::istringstream iss{""};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  for (int i = 0; i < 3; ++i) {
    EXPECT_FALSE(getjson(iss, msg, lineNum, newlines));
    EXPECT_TRUE(iss.eof());
    EXPECT_EQ("", msg);
    EXPECT_EQ(-1, lineNum);
    EXPECT_EQ(0, newlines);
  }
}

TEST(getjson, testPartial) {
  std::istringstream iss{" {{ a }}\n{{ b }  "};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{{ a }}", msg);
  EXPECT_EQ(1, lineNum);
  EXPECT_EQ(0, newlines);

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{{ b }  ", msg);
  EXPECT_EQ(2, lineNum);
  EXPECT_EQ(1, newlines);

  EXPECT_FALSE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("", msg);
  EXPECT_EQ(-1, lineNum);
  EXPECT_EQ(1, newlines);
}

TEST(getjson, mismatchedBraces) {
  std::istringstream iss{" } { a }"};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{ a }", msg);
  EXPECT_EQ(1, lineNum);
  EXPECT_EQ(0, newlines);
}

TEST(getjson, testStrings) {
  std::istringstream iss{" { \"}\n\" }, { \"\\\"}\" } "};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{ \"}\n\" }", msg);
  EXPECT_EQ(1, lineNum);
  EXPECT_EQ(1, newlines);

  EXPECT_TRUE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("{ \"\\\"}\" }", msg);
  EXPECT_EQ(2, lineNum);
  EXPECT_EQ(1, newlines);

  EXPECT_FALSE(getjson(iss, msg, lineNum, newlines));
  EXPECT_EQ("", msg);
  EXPECT_EQ(-1, lineNum);
  EXPECT_EQ(1, newlines);
}

TEST(getjson, testYaml) {
  std::istringstream iss{"test: 1\n---\ntest: 2\n..."};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getyaml(iss, msg, lineNum, newlines));
  EXPECT_EQ("test: 1\n", msg);
  EXPECT_EQ(1, lineNum);
  EXPECT_EQ(2, newlines);

  EXPECT_TRUE(getyaml(iss, msg, lineNum, newlines));
  EXPECT_EQ("test: 2\n", msg);
  EXPECT_EQ(3, lineNum);
  EXPECT_EQ(4, newlines);
}

TEST(getjson, testYamlEmpty) {
  std::istringstream iss{""};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_FALSE(getyaml(iss, msg, lineNum, newlines));
  EXPECT_EQ("", msg);
  EXPECT_EQ(-1, lineNum);
  EXPECT_EQ(0, newlines);
}

TEST(getjson, testYamlPartial) {
  std::istringstream iss{"   \n---\ntest: 1\n---\n---\ntest: 2"};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getyaml(iss, msg, lineNum, newlines));
  EXPECT_EQ("test: 1\n", msg);
  EXPECT_EQ(3, lineNum);
  EXPECT_EQ(4, newlines);

  EXPECT_TRUE(getyaml(iss, msg, lineNum, newlines));
  EXPECT_EQ("test: 2\n", msg);
  EXPECT_EQ(6, lineNum);
  EXPECT_EQ(6, newlines);
}

TEST(getjson, testYamlPartial2) {
  std::istringstream iss{"   \n---\ntest: 1\n---\n---\ntest: 2\ntest: 3\n\n"};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getyaml(iss, msg, lineNum, newlines));
  EXPECT_EQ("test: 1\n", msg);
  EXPECT_EQ(3, lineNum);
  EXPECT_EQ(4, newlines);

  EXPECT_TRUE(getyaml(iss, msg, lineNum, newlines));
  EXPECT_EQ("test: 2\ntest: 3\n\n", msg);
  EXPECT_EQ(6, lineNum);
  EXPECT_EQ(8, newlines);
}

TEST(getjson, testLine) {
  using ofp::yaml::getline;

  std::istringstream iss{"  \ntest 1\n\ntest 2\ntest 3\n\n"};
  int newlines = 0;
  int lineNum = 0;
  std::string msg;

  EXPECT_TRUE(getline(iss, msg, lineNum, newlines));
  EXPECT_EQ("test 1", msg);
  EXPECT_EQ(2, lineNum);
  EXPECT_EQ(2, newlines);

  EXPECT_TRUE(getline(iss, msg, lineNum, newlines));
  EXPECT_EQ("test 2", msg);
  EXPECT_EQ(4, lineNum);
  EXPECT_EQ(4, newlines);

  EXPECT_TRUE(getline(iss, msg, lineNum, newlines));
  EXPECT_EQ("test 3", msg);
  EXPECT_EQ(5, lineNum);
  EXPECT_EQ(5, newlines);

  EXPECT_FALSE(getline(iss, msg, lineNum, newlines));
  EXPECT_EQ("", msg);
  EXPECT_EQ(-1, lineNum);
  EXPECT_EQ(6, newlines);
}
