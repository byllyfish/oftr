#include "ofp/unittest.h"
#include "ofp/strings.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ysmallcstring.h"

using namespace ofp;

static void diagnosticHandler(const llvm::SMDiagnostic &diag, void *context) {
  llvm::errs() << diag.getMessage().str() << '\n';
}

struct TestStructCString {
	TableNameStr str;
};

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<TestStructCString> {
  static void mapping(IO &io, TestStructCString &ts) {
    io.mapRequired("str", ts.str);
  }
};

} // namespace yaml
} // namespace llvm


TEST(ysmallcstring, test_decode) {
  TestStructCString ts;
  std::string input{"str: \"\x1C\ufffd\""};

  llvm::yaml::Input yin{input, nullptr, diagnosticHandler, nullptr};
  yin >> ts;

  EXPECT_FALSE(yin.error());
  EXPECT_EQ("\x1C\xEF\xBF\xBD", ts.str.toString());
}


TEST(ysmallcstring, test_encode) {
  TestStructCString ts;
  ts.str = HexToRawData("E7A781E381AEE38386E383BCE38396E383AB");
  EXPECT_EQ(ts.str, ts.str.toString());

  std::string buf;
  llvm::raw_string_ostream oss{buf};
  llvm::yaml::Output yout{oss};
  yout << ts;

  EXPECT_EQ("---\nstr:             \"\xE7\xA7\x81\xE3\x81\xAE\xE3\x83\x86\xE3\x83\xBC\xE3\x83\x96\xE3\x83\xAB\"\n...\n", oss.str());
}
