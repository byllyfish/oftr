#include <vector>
#include <iostream>
#include <cstdlib>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"

using namespace std;

struct TestStruct {
    int a;
    string b;
    bool c;
};

struct TestContainer {
    std::vector<TestStruct> d;
};

LLVM_YAML_IS_SEQUENCE_VECTOR(TestStruct)

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct MappingTraits<TestStruct> {
    static void mapping(llvm::yaml::IO &io, TestStruct &item)
    {
        io.mapRequired("a", item.a);
        io.mapRequired("b", item.b);
        io.mapRequired("c", item.c);
    }
};

template <>
struct MappingTraits<TestContainer> {
    static void mapping(llvm::yaml::IO &io, TestContainer &item)
    {
        io.mapRequired("d", item.d);
    }
};

} // </namespace yaml>
} // </namespace llvm>

inline void testFailed(const char *condition, const char *file, int linenum)
{
    cerr << "TEST FAILED: `" << condition << "`  (" << file << ":" << linenum
         << ")\n";
    std::exit(1);
}

#define EXPECT(condition)                                                      \
    {                                                                          \
        if (!(condition))                                                      \
            testFailed(#condition, __FILE__, __LINE__);                        \
    }



static void test_YamlParser_MissingEndQuote() {

    {
        const std::string badYaml1 = 
R"""(---
test:
  b:    'no end quote in value position
---
)""";

        llvm::yaml::Input yin(badYaml1);
        EXPECT(!yin.error());

        TestStruct t;
        yin >> t;

        EXPECT(yin.error());
    }

    {
        const std::string badYaml2 = 
R"""(---
test:
  a:  no end quote in key position
 'b:  3
---
)""";

        llvm::yaml::Input yin(badYaml2);
        EXPECT(!yin.error());

        TestStruct t;
        yin >> t;

        EXPECT(yin.error());
    }
}

static void test_Yaml_IsFloat() {
    EXPECT(llvm::yaml::isFloat("0"));
    EXPECT(llvm::yaml::isFloat("0."));
    EXPECT(llvm::yaml::isFloat(".0"));
    EXPECT(llvm::yaml::isFloat("0.0"));
    EXPECT(llvm::yaml::isFloat("0e0"));
    EXPECT(llvm::yaml::isFloat("0E0"));
    EXPECT(llvm::yaml::isFloat("0.e0"));
    EXPECT(llvm::yaml::isFloat(".0e0"));
    EXPECT(llvm::yaml::isFloat("0.0e0"));

    EXPECT(!llvm::yaml::isFloat(""));
    EXPECT(!llvm::yaml::isFloat(" "));
    EXPECT(!llvm::yaml::isFloat("."));
    EXPECT(!llvm::yaml::isFloat("e0"));
    EXPECT(!llvm::yaml::isFloat("0.e"));
    EXPECT(!llvm::yaml::isFloat(".e"));
    EXPECT(!llvm::yaml::isFloat(".0.e0"));
    EXPECT(!llvm::yaml::isFloat(".0."));
    EXPECT(!llvm::yaml::isFloat("0e0 "));
}

static void test_YamlParser_EndBraceOnly() {
    // If the brace is only preceded by spaces, it's an error.
    {
        llvm::SourceMgr sm;
        llvm::yaml::Stream stream("  }", sm);
        EXPECT(!stream.validate());
        EXPECT(stream.failed());
    }

    // If the brace is preceded by a valid char, it's okay.
    {
        llvm::SourceMgr sm;
        llvm::yaml::Stream stream("  a}", sm);
        EXPECT(stream.validate());
        EXPECT(!stream.failed());
    }
}

int main()
{
    {
        TestStruct s = {-54, "it works", true};

        string result;
        llvm::raw_string_ostream rss{result};

        llvm::yaml::Output yout{rss};
        yout << s;

        const char *expected = R"""(---
a:               -54
b:               it works
c:               true
...
)""";

        EXPECT(result.empty());

        string ans = rss.str();
        EXPECT(ans == result);

        cout << result;
        EXPECT(ans == expected);

        TestStruct t;
        llvm::yaml::Input yin(result);
        yin >> t;

        EXPECT(!yin.error());
        EXPECT(t.a == -54);
        EXPECT(t.b == "it works");
        EXPECT(t.c == true);
    }

    {
        const char *json = R"""({ 'a': 72, 'b':'it still works', 'c': true})""";
        TestStruct t;
        llvm::yaml::Input yin(json);
        yin >> t;

        EXPECT(!yin.error());
        EXPECT(t.a == 72);
        EXPECT(t.b == "it still works");
        EXPECT(t.c == true);
    }

    {
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

        EXPECT(t.d.size() == 0);
        // This should return an error; d is an array of structs, but the 
        // above is a map.
        EXPECT(yin.error());
    }

   {
        const char *yml = R"""(---
d: []
...
)""";

        TestContainer t;
        llvm::yaml::Input yin(yml);
        yin >> t;

        EXPECT(t.d.size() == 0);
        EXPECT(!yin.error());
    }


   {
        const char *yml = R"""(---
d: 
...
)""";

        TestContainer t;
        llvm::yaml::Input yin(yml);
        yin >> t;

        EXPECT(t.d.size() == 0);
        EXPECT(!yin.error());
    }

   {
        const char *yml = R"""(---
d: !!null
...
)""";

        TestContainer t;
        llvm::yaml::Input yin(yml);
        yin >> t;

        EXPECT(t.d.size() == 0);
        EXPECT(!yin.error());
    }

   {
        const char *yml = R"""(---
d: null 
...
)""";

        TestContainer t;
        llvm::yaml::Input yin(yml);
        yin >> t;

        EXPECT(t.d.size() == 0);
        EXPECT(!yin.error());
    }

   {
        const char *yml = R"""(---
d: ~ 
...
)""";

        TestContainer t;
        llvm::yaml::Input yin(yml);
        yin >> t;

        EXPECT(t.d.size() == 0);
        EXPECT(!yin.error());
    }

   {
        const char *yml = R"""(---
d: 'null'
...
)""";

        TestContainer t;
        llvm::yaml::Input yin(yml);
        yin >> t;

        EXPECT(t.d.size() == 0);
        EXPECT(!yin.error());
    }

   {
        const char *yml = R"""(---
d: 3
...
)""";

        TestContainer t;
        llvm::yaml::Input yin(yml);
        yin >> t;

        EXPECT(t.d.size() == 0);
        EXPECT(yin.error());
    }

    test_YamlParser_MissingEndQuote();
    test_Yaml_IsFloat();
    test_YamlParser_EndBraceOnly();

    return 0;
}
