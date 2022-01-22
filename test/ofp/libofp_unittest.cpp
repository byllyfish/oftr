// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/libofp.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(libofp, invalid_arguments) {
  ASSERT_EQ(-1, oftr_call(OFTR_VERSION, nullptr, 0, nullptr, 1));
  ASSERT_EQ(-1, oftr_call(10, nullptr, 1, nullptr, 0));
  ASSERT_EQ(-1, oftr_call(OFTR_VERSION, nullptr, 1, nullptr, 1));
  ASSERT_EQ(-1, oftr_call(OFTR_VERSION, nullptr, 0, nullptr, 1));
}

TEST(libofp, oftr_version) {
  char buffer[512];
  int len = oftr_call(OFTR_VERSION, nullptr, 0, buffer, sizeof(buffer));
  ASSERT_GT(len, 0);
  ASSERT_LE(len, sizeof(buffer));

  llvm::StringRef result{buffer, static_cast<size_t>(len)};
  EXPECT_TRUE(result.startswith(LIBOFP_VERSION_STRING));

  // Test passing empty buffers.
  ASSERT_LT(oftr_call(OFTR_VERSION, nullptr, 0, nullptr, 0), -3);

  // Test passing short buffer.
  char small[5];
  ASSERT_LT(oftr_call(OFTR_VERSION, nullptr, 0, small, sizeof(small)), -3);
}

TEST(libofp, oftr_encode) {
  char buffer[128];
  char msg[] = "type: HELLO";
  size_t msglen = strlen(msg);
  UInt32 vers = 1 << 24;

  int len = oftr_call(OFTR_ENCODE_OPENFLOW + vers, msg, msglen, buffer,
                      sizeof(buffer));
  EXPECT_GT(len, 0);
  EXPECT_HEX("0100000800000000", buffer, static_cast<size_t>(len));

  // Test empty input buffer.
  len = oftr_call(OFTR_ENCODE_OPENFLOW + vers, nullptr, 0, buffer,
                  sizeof(buffer));
  EXPECT_LT(len, 0);
  EXPECT_ASCII("YAML:1:1: error: not a document", buffer,
               static_cast<size_t>(-len));

  // Test passing empty buffers.
  EXPECT_EQ(oftr_call(OFTR_ENCODE_OPENFLOW + vers, nullptr, 0, nullptr, 0),
            -31);

  // Test passing empty output buffer.
  EXPECT_EQ(oftr_call(OFTR_ENCODE_OPENFLOW + vers, msg, msglen, nullptr, 0),
            -8);

  // Test passing short output buffer.
  char small[5];
  EXPECT_EQ(
      oftr_call(OFTR_ENCODE_OPENFLOW + vers, msg, msglen, small, sizeof(small)),
      -8);
}

TEST(libofp, oftr_decode) {
  char buffer[128];
  std::string msg = HexToRawData("0100000800000000");

  int len = oftr_call(OFTR_DECODE_OPENFLOW, msg.data(), msg.size(), buffer,
                      sizeof(buffer));
  EXPECT_EQ(len, 101);
  EXPECT_ASCII(
      "---\ntype:            HELLO\nxid:             0x00000000\nversion:      "
      "   0x01\nmsg:             {}\n...\n",
      buffer, static_cast<size_t>(len));

  // Test empty input buffer.
  len = oftr_call(OFTR_DECODE_OPENFLOW, nullptr, 0, buffer, sizeof(buffer));
  EXPECT_EQ(len, -21);
  EXPECT_ASCII("Buffer size < 8 bytes", buffer, static_cast<size_t>(-len));

  // Test passing empty buffers.
  EXPECT_EQ(oftr_call(OFTR_DECODE_OPENFLOW, nullptr, 0, nullptr, 0), -21);

  // Test passing empty output buffer.
  EXPECT_EQ(oftr_call(OFTR_DECODE_OPENFLOW, msg.data(), msg.size(), nullptr, 0),
            -101);

  // Test passing short output buffer.
  char small[5];
  EXPECT_EQ(oftr_call(OFTR_DECODE_OPENFLOW, msg.data(), msg.size(), small,
                      sizeof(small)),
            -101);
}
