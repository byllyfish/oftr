#include <vector>
#include <iostream>
#include <cstdlib>

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"

using namespace std;

struct TestStruct {
    int a;
    string b;
};

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

template <>
struct ScalarTraits<string> {
    static void output(const string &value, void *ctxt, llvm::raw_ostream &out)
    {
        out << value;
    }

    static StringRef input(StringRef scalar, void *ctxt, string &value)
    {
        value = scalar;
        return "";
    }
};

template <>
struct MappingTraits<TestStruct> {
    static void mapping(llvm::yaml::IO &io, TestStruct &item)
    {
        io.mapRequired("a", item.a);
        io.mapRequired("b", item.b);
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

int main()
{
    {
        TestStruct s = {-54, "it works"};

        string result;
        llvm::raw_string_ostream rss{result};

        llvm::yaml::Output yout{rss};
        yout << s;

        const char *expected = R"""(---
a:               -54
b:               it works
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
    }

    {
        const char *json = R"""({ 'a': 72, 'b':'it still works'})""";
        TestStruct t;
        llvm::yaml::Input yin(json);
        yin >> t;

        EXPECT(!yin.error());
        EXPECT(t.a == 72);
        EXPECT(t.b == "it still works");
    }

    return 0;
}
