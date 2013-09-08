#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"
#include "ofp/yaml/encoder.h"

using namespace ofp;
using namespace yaml;

TEST(decoder, test)
{
    auto s = HexToRawData("0100000800000001");

    Message msg{s.data(), s.size()};
    msg.transmogrify();

    std::string result;
    llvm::raw_string_ostream rss{result};
    llvm::yaml::Output yout{rss};

    Decoder decoder{&msg};
    yout << decoder;
    (void)rss.str();

    EXPECT_EQ("", decoder.error());
    EXPECT_EQ("---\ntype:            OFPT_HELLO\nxid:             "
              "0x00000001\nversion:         1\nmsg:             \n  versions:  "
              "      [ 1 ]\n...\n",
              result);

    llvm::yaml::Input yin{result};
    Encoder encoder;
    yin >> encoder;

    EXPECT_EQ("", encoder.error());
    EXPECT_HEX("0100000800000001", encoder.data(), encoder.size());
}

TEST(decoder, test2)
{
    auto s = HexToRawData("04000010000000010001000800000012");

    Message msg{s.data(), s.size()};
    msg.transmogrify();

    std::string result;
    llvm::raw_string_ostream rss{result};
    llvm::yaml::Output yout{rss};

    Decoder decoder{&msg};
    yout << decoder;
    (void)rss.str();

    EXPECT_EQ("", decoder.error());
    EXPECT_EQ("---\ntype:            OFPT_HELLO\nxid:             "
              "0x00000001\nversion:         4\nmsg:             \n  versions:  "
              "      [ 1, 4 ]\n...\n",
              result);

    llvm::yaml::Input yin{result};
    Encoder encoder;
    yin >> encoder;

    EXPECT_EQ("", encoder.error());
    EXPECT_HEX(
        "04000010000000010001000800000012", encoder.data(), encoder.size());
}
