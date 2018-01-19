// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacket.h"
#include "ofp/matchpacketbuilder.h"
#include "ofp/unittest.h"

using namespace ofp;

static void fuzzPacket(const char *hex, size_t start = 0) {
  ByteList buf{HexToRawData(hex)};
  ByteList result;

  EXPECT_GE(buf.size(), 14);

  // Insert two zero bytes at the beginning. MatchPacket expects packet data
  // to be aligned at 2 bytes past the 8-byte alignment.
  buf.insertZeros(buf.begin(), 2);

  const UInt8 kFuzzValues[] = {0x00, 0xFF};

  // Mutate each byte one at a time.
  for (size_t i = start + 2; i < buf.size(); ++i) {
    UInt8 saved = buf[i];

    for (UInt8 val : kFuzzValues) {
      if (buf[i] == val)
        continue;

      buf[i] = val;
      ByteRange pkt{buf.data() + 2, buf.size() - 2};
      MatchPacket match{pkt};
      EXPECT_GT(match.size(), 0);

      MatchPacketBuilder builder{match.toRange()};
      size_t offset = match.toRange().get<X_PKT_POS>();
      if (offset == 0) {
        offset = buf.size() - 2;
      }

      ByteRange remaining =
          SafeByteRange(buf.data() + 2, buf.size() - 2, offset);
      result.clear();
      builder.build(&result, remaining);
      EXPECT_GT(result.size(), 0);
    }

    buf[i] = saved;
  }
}

TEST(matchpacket_fuzz, ethernet) {
  // Ethernet
  fuzzPacket("1111111111112222222222223333");
  fuzzPacket("111111111111222222222222333344");
}

TEST(matchpacket_fuzz, arp) {
  // Arp
  fuzzPacket(
      "111111111111222222222222080600010800060400010000000000010A00000100000000"
      "00000A000002");
}

TEST(matchpacket_fuzz, ipv4) {
  // IPv4
  fuzzPacket(
      "111111111111222222222222080045334444555500007706888899999999AAAAAAAA");
}

TEST(matchpacket_fuzz, tcpv4) {
  // TCPv4
  fuzzPacket(
      "111111111111222222222222080045334444555500007706888899999999AAAAAAAABBBB"
      "CCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFFFFFF");
}

TEST(matchpacket_fuzz, udpv4) {
  // UDPv4
  fuzzPacket(
      "111111111111222222222222080045334444555500007711888899999999AAAAAAAABBBB"
      "CCCCDDDDEEEE");
}

TEST(matchpacket_fuzz, icmpv4) {
  // ICMPv4
  fuzzPacket(
      "111111111111222222222222080045334444555500007701888899999999AAAAAAAABBCC"
      "DDDDDDDD");
}

TEST(matchpacket_fuzz, lldp) {
  // LLDP
  fuzzPacket(
      "0180C200000E00000000000188CC0207040000000000010403072D32060200780A1A4F46"
      "7C30303A30303A30303A30303A30303A30303A30303A3031FE240026E1004F467C2D3240"
      "4F467C30303A30303A30303A30303A30303A30303A30303A30310000");
}

TEST(matchpacket_fuzz, icmpv6) {
  // ICMPv6
  fuzzPacket(
      "3333FF718C4C32FA25D4234186DD6000000000183AFF0000000000000000000000000000"
      "0000FF0200000000000000000001FF718C4C87009A9E00000000FE80000000000000FC0C"
      "CDFFFE718C4C");
}

TEST(matchpacket_fuzz, tcpv6) {
  // TCPv6
  fuzzPacket(
      "11111111111122222222222286DD63344444555511667777777777777777777777777777"
      "777788888888888888888888888888888888BBBBCCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFF"
      "FFFF");
}

TEST(matchpacket_fuzz, udpv6) {
  // UDPv6
  fuzzPacket(
      "11111111111122222222222286DD63344444555511667777777777777777777777777777"
      "777788888888888888888888888888888888BBBBCCCCDDDDEEEE");
}
