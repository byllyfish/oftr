#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"
#include "gtest/gtest.h"

struct TestStruct {
  int a;
  std::string b;
  bool c;
};

struct TestContainer {
  std::vector<TestStruct> d;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(TestStruct)

namespace llvm {  // <namespace llvm>
namespace yaml {  // <namespace yaml>

template <>
struct MappingTraits<TestStruct> {
  static void mapping(llvm::yaml::IO &io, TestStruct &item) {
    io.mapRequired("a", item.a);
    io.mapRequired("b", item.b);
    io.mapRequired("c", item.c);
  }
};

template <>
struct MappingTraits<TestContainer> {
  static void mapping(llvm::yaml::IO &io, TestContainer &item) {
    io.mapRequired("d", item.d);
  }
};

}  // </namespace yaml>
}  // </namespace llvm>

TEST(yamlio, test_it_works) {
  TestStruct s = {-54, "it works", true};

  std::string result;
  llvm::raw_string_ostream rss{result};

  llvm::yaml::Output yout{rss};
  yout << s;

  const char *expected = R"""(---
a:               -54
b:               it works
c:               true
...
)""";

  EXPECT_TRUE(result.empty());

  std::string ans = rss.str();
  EXPECT_EQ(ans, result);

  std::cout << result;
  EXPECT_EQ(ans, expected);

  TestStruct t;
  llvm::yaml::Input yin(result);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, -54);
  EXPECT_EQ(t.b, "it works");
  EXPECT_TRUE(t.c);
}

TEST(yamlio, json_works) {
  const char *json = R"""({ 'a': 72, 'b':'it still works', 'c': true})""";
  TestStruct t;
  llvm::yaml::Input yin(json);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 72);
  EXPECT_EQ(t.b, "it still works");
  EXPECT_TRUE(t.c);
}

TEST(yamlio, map_where_seq_expected) {
  const char *yml = R"""(---
d:
  a: -54
  b: it works
  c: true
...
)""";

  TestContainer t;
  llvm::yaml::Input yin(yml);
  yin >> t;

  EXPECT_EQ(t.d.size(), 0);
  // This should return an error; d is an array of structs, but the
  // above is a map.
  EXPECT_TRUE(yin.error());
}

TEST(yamlio, test_empty_seq) {
  const char *yml = R"""(---
d: []
...
)""";

  TestContainer t;
  llvm::yaml::Input yin(yml);
  yin >> t;

  EXPECT_EQ(t.d.size(), 0);
  EXPECT_FALSE(yin.error());
}

TEST(yamlio, test_null_seq) {
  const char *yml = R"""(---
d: !!null
...
)""";

  TestContainer t;
  llvm::yaml::Input yin(yml);
  yin >> t;

  EXPECT_EQ(t.d.size(), 0);
  EXPECT_FALSE(yin.error());
}

TEST(yamlio, test_null_seq_literal) {
  const char *yml = R"""(---
d: null 
...
)""";

  TestContainer t;
  llvm::yaml::Input yin(yml);
  yin >> t;

  EXPECT_EQ(t.d.size(), 0);
  EXPECT_FALSE(yin.error());
}

TEST(yamlio, test_null_seq_symbol) {
  const char *yml = R"""(---
d: ~ 
...
)""";

  TestContainer t;
  llvm::yaml::Input yin(yml);
  yin >> t;

  EXPECT_EQ(t.d.size(), 0);
  EXPECT_FALSE(yin.error());
}

TEST(yamlio, test_null_seq_str) {
  const char *yml = R"""(---
d: 'null'
...
)""";

  TestContainer t;
  llvm::yaml::Input yin(yml);
  yin >> t;

  EXPECT_EQ(t.d.size(), 0);
  EXPECT_FALSE(yin.error());
}

TEST(yamlio, test_null_seq_invalid) {
  const char *yml = R"""(---
d: 3
...
)""";

  TestContainer t;
  llvm::yaml::Input yin(yml);
  yin >> t;

  EXPECT_EQ(t.d.size(), 0);
  EXPECT_TRUE(yin.error());
}

TEST(yamlio, missing_end_quote_value_position) {
  const std::string badYaml1 =
      R"""(---
test:
  b:    'no end quote in value position
---
)""";

  llvm::yaml::Input yin(badYaml1);
  EXPECT_FALSE(yin.error());

  TestStruct t;
  yin >> t;

  EXPECT_TRUE(yin.error());
}

TEST(yamlio, missing_end_quote_key_position) {
  const std::string badYaml2 =
      R"""(---
test:
  a:  no end quote in key position
 'b:  3
---
)""";

  llvm::yaml::Input yin(badYaml2);
  EXPECT_FALSE(yin.error());

  TestStruct t;
  yin >> t;

  EXPECT_TRUE(yin.error());
}

TEST(yamlio, isFloat) {
  EXPECT_TRUE(llvm::yaml::isFloat("0"));
  EXPECT_TRUE(llvm::yaml::isFloat("0."));
  EXPECT_TRUE(llvm::yaml::isFloat(".0"));
  EXPECT_TRUE(llvm::yaml::isFloat("0.0"));
  EXPECT_TRUE(llvm::yaml::isFloat("0e0"));
  EXPECT_TRUE(llvm::yaml::isFloat("0E0"));
  EXPECT_TRUE(llvm::yaml::isFloat("0.e0"));
  EXPECT_TRUE(llvm::yaml::isFloat(".0e0"));
  EXPECT_TRUE(llvm::yaml::isFloat("0.0e0"));

  EXPECT_FALSE(llvm::yaml::isFloat(""));
  EXPECT_FALSE(llvm::yaml::isFloat(" "));
  EXPECT_FALSE(llvm::yaml::isFloat("."));
  EXPECT_FALSE(llvm::yaml::isFloat("e0"));
  EXPECT_FALSE(llvm::yaml::isFloat("0.e"));
  EXPECT_FALSE(llvm::yaml::isFloat(".e"));
  EXPECT_FALSE(llvm::yaml::isFloat(".0.e0"));
  EXPECT_FALSE(llvm::yaml::isFloat(".0."));
  EXPECT_FALSE(llvm::yaml::isFloat("0e0 "));
}

static void test_yaml_input_fail(const char *yaml) {
  llvm::SourceMgr sm;
  llvm::yaml::Stream stream(yaml, sm);
  EXPECT_FALSE(stream.validate());
  EXPECT_TRUE(stream.failed());
}

static void test_yaml_input_success(const char *yaml) {
  llvm::SourceMgr sm;
  llvm::yaml::Stream stream(yaml, sm);
  EXPECT_TRUE(stream.validate());
  EXPECT_FALSE(stream.failed());
}

TEST(yamlio, test_various_input) {
  // If the brace is only preceded by spaces, it's an error.
  test_yaml_input_fail("  }");

  // If the brace is preceded by a valid char, it's okay.
  test_yaml_input_success("  a}");

  // If the bracket is only preceded by spaces, it's an error.
  test_yaml_input_fail("  ]");

  // If the bracket is preceded by a valid char, it's okay.
  test_yaml_input_success("  a]");

  // If there are two ore more tags, it's an error.
  test_yaml_input_fail("  !opt !opt");

  // If there are two or more anchors, it's an error.
  test_yaml_input_fail("  &opt &opt");
}
