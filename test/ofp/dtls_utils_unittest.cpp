// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/dtls_utils.h"
#include "ofp/unittest.h"

using namespace ofp;
using namespace ofp::sys;

TEST(dtls_utils, DTLS_GetRecordLength) {
  EXPECT_EQ(0, DTLS_GetRecordLength(nullptr, 0));

  // Too short
  std::string buf1 = HexToRawData("16FEFF00000000000000000001");
  EXPECT_EQ(0, DTLS_GetRecordLength(buf1.data(), buf1.size()));

  // DTLS1.0
  std::string buf2 = HexToRawData("16FEFF00000000000000000001FF");
  EXPECT_EQ(14, DTLS_GetRecordLength(buf2.data(), buf2.size()));

  // Invalid version
  std::string buf3 = HexToRawData("16FEFE00000000000000000001FF");
  EXPECT_EQ(0, DTLS_GetRecordLength(buf3.data(), buf3.size()));

  // DTLS1.2
  std::string buf4 = HexToRawData("16FEFD00000000000000000001FF");
  EXPECT_EQ(14, DTLS_GetRecordLength(buf4.data(), buf4.size()));

  // DTLS1.2 zero length (plus extra byte).
  std::string buf5 = HexToRawData("16FEFD00000000000000000000FF");
  EXPECT_EQ(13, DTLS_GetRecordLength(buf5.data(), buf5.size()));
}

TEST(dtls_utils, DTLS_PrintRecord) {
  // DTLS1.0
  std::string buf1 = HexToRawData("16FEFF00000000000000000001FF");
  EXPECT_EQ("DTLS1.0 handshake(22) len=14 epoch=0 seq=0 [truncated]",
            DTLS_PrintRecord(buf1.data(), buf1.size()));

  // DTLS1.2
  std::string buf2 = HexToRawData("16FEFD00000000000000000001FF");
  EXPECT_EQ("DTLS1.2 handshake(22) len=14 epoch=0 seq=0 [truncated]",
            DTLS_PrintRecord(buf2.data(), buf2.size()));

  // DTLS 1.0
  std::string buf3 = HexToRawData(
      "16FEFF0000000000000000001F030000130000000000000013FEFF100000000000000000"
      "0000000000000000");
  EXPECT_EQ(
      "DTLS1.0 handshake(22) len=44 epoch=0 seq=0 hello_verify_request(3) "
      "msgLen=19 msgSeq=0 fragmentOffset=0 (19 bytes) "
      "cookie=00000000000000000000000000000000",
      DTLS_PrintRecord(buf3.data(), buf3.size()));
}
