#include <cassert>
#include <vector>
#include <iostream>

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"

using namespace std;

struct TestStruct {
    int a;
    string b;
};

template <>
struct llvm::yaml::ScalarTraits<string> {
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
struct llvm::yaml::MappingTraits<TestStruct> {
    static void mapping(llvm::yaml::IO &io, TestStruct &item)
    {
        io.mapRequired("a", item.a);
        io.mapRequired("b", item.b);
    }
};

int main()
{
	{
	    TestStruct s = {-54, "it works"};

	    string result;
	    llvm::raw_string_ostream rss{result};

	    llvm::yaml::Output yout{rss};
	    yout << s;

	    const char *expected = 
R"""(---
a:               -54
b:               it works
...
)""";

		assert(result.empty());

		string ans = rss.str();
		assert(ans == result);
	    
	    cout << result;
	    assert(ans == expected);

	    TestStruct t;
	    llvm::yaml::Input yin(result);
    	yin >> t;

    	assert(!yin.error());
    	assert(t.a == -54);
    	assert(t.b == "it works");
	}

	return 0;
}
