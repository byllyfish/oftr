#include "ofp/unittest.h"
#include "ofp/yaml/enumconverter.h"

using namespace ofp;

TEST(enumconverter, convert) {

  enum Kind {
    A,
    B,
    C,
    UNK
  };

  const char *const names[] = {"A", "B", "C"};
  auto converter = yaml::MakeEnumConverter<Kind>(names);

  {
    Kind k;
    EXPECT_TRUE(converter.convert("A", &k));
    EXPECT_EQ(A, k);

    const char *n;
    EXPECT_TRUE(converter.convert(A, &n));
    EXPECT_EQ("A", n);
  }

  {
    Kind k;
    EXPECT_TRUE(converter.convert("C", &k));
    EXPECT_EQ(C, k);

    const char *n;
    EXPECT_TRUE(converter.convert(C, &n));
    EXPECT_EQ("C", n);
  }
}

TEST(enumconverter, convert_case_senstive) {

  enum Kind {
    A,
    B,
    C,
    KIND_LAST = C
  };

  const char *const names[KIND_LAST + 1] = {"A", "B", "C"};

  auto converter = yaml::MakeEnumConverter<Kind>(names);

  Kind k;
  EXPECT_FALSE(converter.convert("b", &k));

  const char *n;
  EXPECT_TRUE(converter.convert(B, &n));
  EXPECT_EQ("B", n);
}
