#include <vector>
#include <iostream>
#include <cstdlib>

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"

using namespace std;

struct TestStruct {
    int a;
    string b;
    bool c;
};

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

    return 0;
}
