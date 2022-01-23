// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/ybyteorder.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(ybyteorder, Big32) {
  Big32 value;
  llvm::StringRef err =
      llvm::yaml::ScalarTraits<Big32>::input("0x01020304", nullptr, value);
  EXPECT_EQ("", err);
  EXPECT_EQ(0x01020304, value);

  std::string buf;
  llvm::raw_string_ostream oss{buf};
  llvm::yaml::ScalarTraits<Big32>::output(value, nullptr, oss);
  EXPECT_EQ("0x01020304", oss.str());

  err = llvm::yaml::ScalarTraits<Big32>::input("0x0102030405", nullptr, value);
  EXPECT_EQ("out of range number", err.str());
}

TEST(ybyteorder, Big24) {
  Big24 value;
  llvm::StringRef err =
      llvm::yaml::ScalarTraits<Big24>::input("0x010203", nullptr, value);
  EXPECT_EQ("", err);
  EXPECT_EQ(0x010203, value);

  std::string buf;
  llvm::raw_string_ostream oss{buf};
  llvm::yaml::ScalarTraits<Big24>::output(value, nullptr, oss);
  // Continue to output as 32-bits hex.
  EXPECT_EQ("0x00010203", oss.str());

  err = llvm::yaml::ScalarTraits<Big24>::input("0x01020304", nullptr, value);
  EXPECT_EQ("out of range number", err.str());
}
