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


TEST(yamlio, test_unicode_escapes) {
  const char *input = R"""(---
a:               99
b:               " \xff \u0f0f \U0010ffff "
c:               false
...
)""";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 99);
  EXPECT_EQ(t.b, u8" \u00ff \u0f0f \U0010ffff ");
  EXPECT_FALSE(t.c);

  std::string result;
  llvm::raw_string_ostream rss{result};
  llvm::yaml::Output yout{rss};
  yout << t;
  EXPECT_EQ(rss.str(), u8"---\na:               99\nb:               ' \u00ff \u0f0f \U0010ffff '\nc:               false\n...\n");
}

TEST(yamlio, test_invalid_unicode_escapes) {
  const char *input = R"""(---
a:               99
b:               " \u0fhh\U0010fffh \u0f"
c:               false
...
)""";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 99);
  EXPECT_EQ(t.b, " \xEF\xBF\xBD\xEF\xBF\xBD 0f");  // FIXME: THIS IS '0xFFFD'
  EXPECT_FALSE(t.c);
}

TEST(yamlio, test_low_surrogate_escaped) {
  const char *input = R"""(---
a:               99
b:               " \udcfe "
c:               false
...
)""";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 99);
  EXPECT_EQ(t.b, " \xED\xB3\xBE ");  // FIXME
  EXPECT_FALSE(t.c);

  std::string result;
  llvm::raw_string_ostream rss{result};
  llvm::yaml::Output yout{rss};
  yout << t;
  EXPECT_EQ(rss.str(), "---\na:               99\nb:               ' \xED\xB3\xBE '\nc:               false\n...\n");
}

TEST(yamlio, test_surrogate_pair_escaped) {
  const char *input = R"""(---
a:               99
b:               " \ud800\udc00 "
c:               false
...
)""";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 99);
  EXPECT_EQ(t.b, " \xED\xA0\x80\xED\xB0\x80 ");
  EXPECT_FALSE(t.c);

  std::string result;
  llvm::raw_string_ostream rss{result};
  llvm::yaml::Output yout{rss};
  yout << t;
  EXPECT_EQ(rss.str(), "---\na:               99\nb:               ' \xED\xA0\x80\xED\xB0\x80 '\nc:               false\n...\n");
}

TEST(yamlio, test_high_surrogate_escaped) {
  const char *input = R"""(---
a:               077
b:               " \ud8ef "
c:               false
...
)""";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 63);
  EXPECT_EQ(t.b, " \xED\xA3\xAF ");
  EXPECT_FALSE(t.c);

  std::string result;
  llvm::raw_string_ostream rss{result};
  llvm::yaml::Output yout{rss};
  yout << t;
  EXPECT_EQ(rss.str(), "---\na:               63\nb:               ' \xED\xA3\xAF '\nc:               false\n...\n");
}

TEST(yamlio, test_illegal_utf8_surrogate_pair) {
  const char *input = u8"{ a: 0o76, b: \"\xD8\x01\xDC\x37\", c: false }";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 62);
  EXPECT_EQ(t.b, "\xD8\x01\xDC\x37");
  EXPECT_FALSE(t.c);

  std::string result;
  llvm::raw_string_ostream rss{result};
  llvm::yaml::Output yout{rss};
  yout << t;
  EXPECT_EQ(rss.str(), "---\na:               62\nb:               \"\xEF\xBF\xBD\"\nc:               false\n...\n");
}

TEST(yamlio, test_illegal_utf8_low_surrogate) {
  const char *input = "{ a: 91, b: \"\xDC\xFE\", c: false }";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.a, 91);
  EXPECT_EQ(t.b, "\xDC\xFE");
  EXPECT_FALSE(t.c);

  std::string result;
  llvm::raw_string_ostream rss{result};
  llvm::yaml::Output yout{rss};
  yout << t;
  EXPECT_EQ(rss.str(), "---\na:               91\nb:               \"\xEF\xBF\xBD\"\nc:               false\n...\n");
}

TEST(yamlio, test_invalid_octal) {
  const char *input = u8"{ a: 099, b: \"\", c: false }";

  TestStruct t;
  llvm::yaml::Input yin(input);
  yin >> t;

  EXPECT_TRUE(yin.error());
}

TEST(yamlio, test_multiline_flow_list) {
  const char *input = R"""(---
  [ x , 
    y
    ,
    z  
  z
  ]
...
)""";

  std::string buf;
  llvm::raw_string_ostream oss{buf};
  llvm::yaml::dumpTokens(input, oss);

  std::string result = R"""(Stream-Start: 
Document-Start: ---
Flow-Sequence-Start: [
Scalar: x
Flow-Entry: ,
Scalar: y
Flow-Entry: ,
Scalar: z  
  z
Flow-Sequence-End: ]
Document-End: ...
Stream-End: 
)""";

  EXPECT_EQ(oss.str(), result);

  std::vector<std::string> t;
  llvm::yaml::Input yin(input);
  EXPECT_FALSE(yin.error());
  yin >> t;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ(t.size(), 3);
  EXPECT_EQ(t[0], "x");
  EXPECT_EQ(t[1], "y");
  EXPECT_EQ(t[2], "z  \n  z");
}
