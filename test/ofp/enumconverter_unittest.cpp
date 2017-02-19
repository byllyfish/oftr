// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/enumconverter.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(enumconverter, convert) {
  enum Kind { A, B, C, UNK };

  const llvm::StringRef names[] = {"A", "B", "C"};
  ofp::yaml::EnumConverter<Kind> converter{names};

  {
    Kind k = UNK;
    EXPECT_TRUE(converter.convert("A", &k));
    EXPECT_EQ(A, k);

    llvm::StringRef n;
    EXPECT_TRUE(converter.convert(A, &n));
    EXPECT_EQ("A", n.str());
  }

  {
    Kind k = UNK;
    EXPECT_TRUE(converter.convert("C", &k));
    EXPECT_EQ(C, k);

    llvm::StringRef n;
    EXPECT_TRUE(converter.convert(C, &n));
    EXPECT_EQ("C", n.str());
  }
}

TEST(enumconverter, convert_case_senstive) {
  enum Kind { A, B, C, KIND_LAST = C };

  const llvm::StringRef names[KIND_LAST + 1] = {"A", "B", "C"};

  ofp::yaml::EnumConverter<Kind> converter{names};

  Kind k;
  EXPECT_FALSE(converter.convert("b", &k));

  llvm::StringRef n;
  EXPECT_TRUE(converter.convert(B, &n));
  EXPECT_EQ("B", n.str());
}
