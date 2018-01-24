// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/matchpacket.h"
#include "ofp/unittest.h"
#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ymatchpacket.h"

using namespace ofp;

static std::string decodeFields(MatchPacket &fields) {
  std::string result;
  llvm::raw_string_ostream rss{result};
  llvm::yaml::Output yout{rss};
  yout << fields;
  return rss.str();
}

static void testPacket(const char *hex, const char *decodedFields) {
  ByteList buf{HexToRawData(hex)};

  // Insert two zero bytes at the beginning. MatchPacket expects packet data
  // to be aligned at 2 bytes past the 8-byte alignment.
  buf.insertZeros(buf.begin(), 2);

  ASSERT_TRUE(IsPtrAligned(buf.data(), 8));

  MatchPacket match{ByteRange{buf.data() + 2, buf.size() - 2}};
  EXPECT_TRUE(match.toRange().validateInput());
  EXPECT_EQ(decodeFields(match), decodedFields);
}

TEST(matchpacket, ethernet) {
  testPacket("FFFFFFFFFFFF0000000000010806",
             "---\n- field:           ETH_DST\n  value:           "
             "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:        "
             "   '00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:    "
             "       0x0806\n...\n");

  // Unknown type
  testPacket("FFFFFFFFFFFF0000000000010807",
             "---\n- field:           ETH_DST\n  value:           "
             "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:        "
             "   '00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:    "
             "       0x0807\n...\n");

  // 1 byte short
  testPacket(
      "FFFFFFFFFFFF00000000000108",
      "---\n- field:           X_PKT_POS\n  value:           0x0000\n...\n");

  // 1 byte long
  testPacket("FFFFFFFFFFFF000000000001080601",
             "---\n- field:           ETH_DST\n  value:           "
             "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:        "
             "   '00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:    "
             "       0x0806\n- field:           X_PKT_POS\n  value:           "
             "0x000E\n...\n");

  // Vlan header missing
  testPacket("FFFFFFFFFFFF0000000000018100",
             "---\n- field:           ETH_DST\n  value:           "
             "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:        "
             "   '00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:    "
             "       0x8100\n...\n");

  // Vlan header too small
  testPacket("FFFFFFFFFFFF0000000000018100 000102",
             "---\n- field:           ETH_DST\n  value:           "
             "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:        "
             "   '00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:    "
             "       0x8100\n- field:           X_PKT_POS\n  value:           "
             "0x000E\n...\n");

  // Empty packet padded to 60 bytes.
  testPacket(
      "000000000000000000000000000061316232000000000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'00:00:00:00:00:00'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:00'\n- field:           ETH_TYPE\n  value:           "
      "0x0000\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");
}

TEST(matchpacket, arp) {
  testPacket(
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A000002",
      "---\n- field:           ETH_DST\n  value:           "
      "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x0806\n- field:           ARP_OP\n  value:           0x0001\n- field:  "
      "         ARP_SPA\n  value:           10.0.0.1\n- field:           "
      "ARP_TPA\n  value:           10.0.0.2\n- field:           ARP_SHA\n  "
      "value:           '00:00:00:00:00:01'\n- field:           ARP_THA\n  "
      "value:           '00:00:00:00:00:00'\n...\n");

  // 1 byte short
  testPacket(
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A0000",
      "---\n- field:           ETH_DST\n  value:           "
      "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x0806\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");

  // 1 byte long
  testPacket(
      "FFFFFFFFFFFF000000000001080600010800060400010000000000010A00000100000000"
      "00000A00000201",
      "---\n- field:           ETH_DST\n  value:           "
      "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x0806\n- field:           ARP_OP\n  value:           0x0001\n- field:  "
      "         ARP_SPA\n  value:           10.0.0.1\n- field:           "
      "ARP_TPA\n  value:           10.0.0.2\n- field:           ARP_SHA\n  "
      "value:           '00:00:00:00:00:01'\n- field:           ARP_THA\n  "
      "value:           '00:00:00:00:00:00'\n- field:           X_PKT_POS\n  "
      "value:           0x002A\n...\n");

  // Unexpected arp prefix
  testPacket(
      "FFFFFFFFFFFF000000000001080600010800060500010000000000010A00000100000000"
      "00000A000002",
      "---\n- field:           ETH_DST\n  value:           "
      "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x0806\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");

  // Padded with zeros to 60 bytes
  testPacket(
      "FFFFFFFFFFFF0E0000000001810000FA080600010800060400010E0000000001A30789FE"
      "000000000000A30789790000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:           "
      "'0e:00:00:00:00:01'\n- field:           VLAN_VID\n  value:           "
      "0x10FA\n- field:           ETH_TYPE\n  value:           0x0806\n- "
      "field:           ARP_OP\n  value:           0x0001\n- field:           "
      "ARP_SPA\n  value:           163.7.137.254\n- field:           ARP_TPA\n "
      " value:           163.7.137.121\n- field:           ARP_SHA\n  value:   "
      "        '0e:00:00:00:00:01'\n- field:           ARP_THA\n  value:       "
      "    '00:00:00:00:00:00'\n...\n");

  // Padded with _almost_ all zeros to 60 bytes
  testPacket(
      "FFFFFFFFFFFF0E0000000001810000FA080600010800060400010E0000000001A30789FE"
      "000000000000A30789790000000000000000000000000001",
      "---\n- field:           ETH_DST\n  value:           "
      "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:           "
      "'0e:00:00:00:00:01'\n- field:           VLAN_VID\n  value:           "
      "0x10FA\n- field:           ETH_TYPE\n  value:           0x0806\n- "
      "field:           ARP_OP\n  value:           0x0001\n- field:           "
      "ARP_SPA\n  value:           163.7.137.254\n- field:           ARP_TPA\n "
      " value:           163.7.137.121\n- field:           ARP_SHA\n  value:   "
      "        '0e:00:00:00:00:01'\n- field:           ARP_THA\n  value:       "
      "    '00:00:00:00:00:00'\n- field:           X_PKT_POS\n  value:         "
      "  0x002E\n...\n");

  // Minimum arp (60 bytes)
  testPacket(
      "000000000000000000000000080600010800060400000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'00:00:00:00:00:00'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:00'\n- field:           ETH_TYPE\n  value:           "
      "0x0806\n- field:           ARP_OP\n  value:           0x0000\n- field:  "
      "         ARP_SPA\n  value:           0.0.0.0\n- field:           "
      "ARP_TPA\n  value:           0.0.0.0\n- field:           ARP_SHA\n  "
      "value:           '00:00:00:00:00:00'\n- field:           ARP_THA\n  "
      "value:           '00:00:00:00:00:00'\n...\n");
}

TEST(matchpacket, arp_vlan) {
  // Padded with zeros to 60 bytes
  testPacket(
      "0000000000020000000000018100E00108060001080006040001000000000001C0A80101"
      "000000000002C0A801020000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'00:00:00:00:00:02'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           VLAN_VID\n  value:           "
      "0x1001\n- field:           VLAN_PCP\n  value:           0x07\n- field:  "
      "         ETH_TYPE\n  value:           0x0806\n- field:           "
      "ARP_OP\n  value:           0x0001\n- field:           ARP_SPA\n  value: "
      "          192.168.1.1\n- field:           ARP_TPA\n  value:           "
      "192.168.1.2\n- field:           ARP_SHA\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ARP_THA\n  value:           "
      "'00:00:00:00:00:02'\n...\n");
}

TEST(matchpacket, ipv4) {
  // Valid IPv4 Header only
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007706888899999999AAAAAAAA ",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x06\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n...\n");

  // Wrong IP version
  testPacket(
      "1111111111112222222222220800 "
      "55334444555500007706888899999999AAAAAAAA ",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");

  // IP header too short
  testPacket(
      "1111111111112222222222220800 "
      "44334444555500007706888899999999AAAAAAAA ",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");

  // IPv4 fragment
  testPacket(
      "1111111111112222222222220800 "
      "453344445555EDDD7706888899999999AAAAAAAA ",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x06\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_FRAG\n  value:           "
      "0x03\n- field:           NX_IP_TTL\n  value:           0x77\n...\n");

  // IP header longer than remaining data
  testPacket(
      "1111111111112222222222220800 "
      "49334444555500007706888899999999AAAAAAAA 12345678",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");

  // IP header one byte too short
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007706888899999999AAAAAA",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");

  // Unknown IPv4 protocol
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007791888899999999AAAAAAAA ",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x91\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n...\n");
}

TEST(matchpacket, ipv4_totalLen) {
  // Total Length < hdrLen
  testPacket(
      "1111111111112222222222220800 "
      "45330011555500007706888899999999AAAAAAAA ",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           X_PKT_POS\n  value:           0x000E\n...\n");

  // Total Length < data length
  testPacket(
      "1111111111112222222222220800 "
      "45330015555500007706888899999999AAAAAAAA 01020304",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x06\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           X_PKT_POS\n  value:           0x0022\n...\n");
}

TEST(matchpacket, tcpv4) {
  // exact size
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007706888899999999AAAAAAAA "
      "BBBBCCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFFFFFF",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x06\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           TCP_SRC\n  value:           0xBBBB\n- field:   "
      "        TCP_DST\n  value:           0xCCCC\n- field:           "
      "NX_TCP_FLAGS\n  value:           0x0EEE\n...\n");

  // 1 byte long (todo: check ipv4 len?)
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007706888899999999AAAAAAAA "
      "BBBBCCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFFFFFF 01",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x06\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           TCP_SRC\n  value:           0xBBBB\n- field:   "
      "        TCP_DST\n  value:           0xCCCC\n- field:           "
      "NX_TCP_FLAGS\n  value:           0x0EEE\n- field:           X_PKT_POS\n "
      " value:           0x0036\n...\n");

  // 1 byte short
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007706888899999999AAAAAAAA "
      "BBBBCCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFFFF",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x06\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           X_PKT_POS\n  value:           0x0022\n...\n");
}

TEST(matchpacket, udpv4) {
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007711888899999999AAAAAAAA BBBBCCCCDDDDEEEE",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x11\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           UDP_SRC\n  value:           0xBBBB\n- field:   "
      "        UDP_DST\n  value:           0xCCCC\n...\n");
}

TEST(matchpacket, icmpv4) {
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007701888899999999AAAAAAAA BBCCDDDDDDDD",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x01\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           ICMPV4_TYPE\n  value:           0xBB\n- field: "
      "          ICMPV4_CODE\n  value:           0xCC\n- field:           "
      "X_PKT_POS\n  value:           0x0026\n...\n");

  // exact size
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007701888899999999AAAAAAAA BBCCDDDD",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x01\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           ICMPV4_TYPE\n  value:           0xBB\n- field: "
      "          ICMPV4_CODE\n  value:           0xCC\n...\n");

  // 1 byte short
  testPacket(
      "1111111111112222222222220800 "
      "45334444555500007701888899999999AAAAAAAA BBCCDD",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           IP_PROTO\n  "
      "value:           0x01\n- field:           IPV4_SRC\n  value:           "
      "153.153.153.153\n- field:           IPV4_DST\n  value:           "
      "170.170.170.170\n- field:           NX_IP_TTL\n  value:           "
      "0x77\n- field:           X_PKT_POS\n  value:           0x0022\n...\n");
}

TEST(matchpacket, icmpv6) {
  testPacket(
      "3333FF718C4C32FA25D4234186DD6000000000183AFF0000000000000000000000000000"
      "0000FF0200000000000000000001FF718C4C87009A9E00000000FE80000000000000FC0C"
      "CDFFFE718C4C",
      "---\n- field:           ETH_DST\n  value:           "
      "'33:33:ff:71:8c:4c'\n- field:           ETH_SRC\n  value:           "
      "'32:fa:25:d4:23:41'\n- field:           ETH_TYPE\n  value:           "
      "0x86DD\n- field:           NX_IP_TTL\n  value:           0xFF\n- field: "
      "          IP_PROTO\n  value:           0x3A\n- field:           "
      "IPV6_SRC\n  value:           '::'\n- field:           IPV6_DST\n  "
      "value:           'ff02::1:ff71:8c4c'\n- field:           ICMPV6_TYPE\n  "
      "value:           0x87\n- field:           ICMPV6_CODE\n  value:         "
      "  0x00\n- field:           IPV6_ND_TARGET\n  value:           "
      "'fe80::fc0c:cdff:fe71:8c4c'\n- field:           IPV6_EXTHDR\n  value:   "
      "        0x0001\n...\n");

  // exact size
  testPacket(
      "3333FF718C4C32FA25D4234186DD6000000000183AFF0000000000000000000000000000"
      "0000FF0200000000000000000001FF718C4C87009A9E",
      "---\n- field:           ETH_DST\n  value:           "
      "'33:33:ff:71:8c:4c'\n- field:           ETH_SRC\n  value:           "
      "'32:fa:25:d4:23:41'\n- field:           ETH_TYPE\n  value:           "
      "0x86DD\n- field:           NX_IP_TTL\n  value:           0xFF\n- field: "
      "          IP_PROTO\n  value:           0x3A\n- field:           "
      "IPV6_SRC\n  value:           '::'\n- field:           IPV6_DST\n  "
      "value:           'ff02::1:ff71:8c4c'\n- field:           ICMPV6_TYPE\n  "
      "value:           0x87\n- field:           ICMPV6_CODE\n  value:         "
      "  0x00\n- field:           IPV6_EXTHDR\n  value:           "
      "0x0001\n...\n");

  // 1 byte short
  testPacket(
      "3333FF718C4C32FA25D4234186DD6000000000183AFF0000000000000000000000000000"
      "0000FF0200000000000000000001FF718C4C87009A",
      "---\n- field:           ETH_DST\n  value:           "
      "'33:33:ff:71:8c:4c'\n- field:           ETH_SRC\n  value:           "
      "'32:fa:25:d4:23:41'\n- field:           ETH_TYPE\n  value:           "
      "0x86DD\n- field:           NX_IP_TTL\n  value:           0xFF\n- field: "
      "          IP_PROTO\n  value:           0x3A\n- field:           "
      "IPV6_SRC\n  value:           '::'\n- field:           IPV6_DST\n  "
      "value:           'ff02::1:ff71:8c4c'\n- field:           IPV6_EXTHDR\n  "
      "value:           0x0001\n- field:           X_PKT_POS\n  value:         "
      "  0x0036\n...\n");
}

TEST(matchpacket, tcpv6) {
  testPacket(
      "11111111111122222222222286DD "
      "633444445555116677777777777777777777777777777777888888888888888888888888"
      "88888888 BBBBCCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFFFFFF",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x86DD\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           NX_IP_TTL\n "
      " value:           0x66\n- field:           IP_PROTO\n  value:           "
      "0x11\n- field:           IPV6_SRC\n  value:           "
      "'7777:7777:7777:7777:7777:7777:7777:7777'\n- field:           "
      "IPV6_DST\n  value:           "
      "'8888:8888:8888:8888:8888:8888:8888:8888'\n- field:           "
      "IPV6_FLABEL\n  value:           0x00044444\n- field:           "
      "UDP_SRC\n  value:           0xBBBB\n- field:           UDP_DST\n  "
      "value:           0xCCCC\n- field:           IPV6_EXTHDR\n  value:       "
      "    0x0001\n- field:           X_PKT_POS\n  value:           "
      "0x003E\n...\n");
}

TEST(matchpacket, udpv6) {
  testPacket(
      "11111111111122222222222286DD "
      "633444445555116677777777777777777777777777777777888888888888888888888888"
      "88888888 BBBBCCCCDDDDEEEE",
      "---\n- field:           ETH_DST\n  value:           "
      "'11:11:11:11:11:11'\n- field:           ETH_SRC\n  value:           "
      "'22:22:22:22:22:22'\n- field:           ETH_TYPE\n  value:           "
      "0x86DD\n- field:           IP_DSCP\n  value:           0x0C\n- field:   "
      "        IP_ECN\n  value:           0x03\n- field:           NX_IP_TTL\n "
      " value:           0x66\n- field:           IP_PROTO\n  value:           "
      "0x11\n- field:           IPV6_SRC\n  value:           "
      "'7777:7777:7777:7777:7777:7777:7777:7777'\n- field:           "
      "IPV6_DST\n  value:           "
      "'8888:8888:8888:8888:8888:8888:8888:8888'\n- field:           "
      "IPV6_FLABEL\n  value:           0x00044444\n- field:           "
      "UDP_SRC\n  value:           0xBBBB\n- field:           UDP_DST\n  "
      "value:           0xCCCC\n- field:           IPV6_EXTHDR\n  value:       "
      "    0x0001\n...\n");
}

TEST(matchpacket, lldp) {
  testPacket(
      "0180C200000E00000000000188CC0207040000000000010403072D32060200780A1A4F46"
      "7C30303A30303A30303A30303A30303A30303A30303A3031FE240026E1004F467C2D3240"
      "4F467C30303A30303A30303A30303A30303A30303A30303A30310000",
      "---\n- field:           ETH_DST\n  value:           "
      "'01:80:c2:00:00:0e'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x88CC\n- field:           X_LLDP_CHASSIS_ID\n  value:           'mac "
      "000000000001'\n- field:           X_LLDP_PORT_ID\n  value:           "
      "'-2'\n- field:           X_LLDP_TTL\n  value:           0x0078\n- "
      "field:           X_LLDP_SYS_NAME\n  value:           "
      "'OF|00:00:00:00:00:00:00:01'\n- field:           X_LLDP_CUSTOM1\n  "
      "value:           '0x26e1 0x0 "
      "4F467C2D32404F467C30303A30303A30303A30303A30303A30303A30303A3031'\n..."
      "\n");

  // malformed lldp tlv
  testPacket(
      "0180C200000EB2D434D89B0388CC0209010000000000000002040502000000020603000"
      "1",
      "---\n- field:           ETH_DST\n  value:           "
      "'01:80:c2:00:00:0e'\n- field:           ETH_SRC\n  value:           "
      "'b2:d4:34:d8:9b:03'\n- field:           ETH_TYPE\n  value:           "
      "0x88CC\n- field:           X_LLDP_CHASSIS_ID\n  value:           "
      "'chassis 0000000000000002'\n- field:           X_LLDP_PORT_ID\n  value: "
      "          'port 00000002'\n- field:           X_PKT_POS\n  value:       "
      "    0x0020\n...\n");

  // no end tlv
  testPacket(
      "0180C200000E00000000000188CC0207040000000000010403072D32060200780A1A4F46"
      "7C30303A30303A30303A30303A30303A30303A30303A3031FE240026E1004F467C2D3240"
      "4F467C30303A30303A30303A30303A30303A30303A30303A3031",
      "---\n- field:           ETH_DST\n  value:           "
      "'01:80:c2:00:00:0e'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x88CC\n- field:           X_LLDP_CHASSIS_ID\n  value:           'mac "
      "000000000001'\n- field:           X_LLDP_PORT_ID\n  value:           "
      "'-2'\n- field:           X_LLDP_TTL\n  value:           0x0078\n- "
      "field:           X_LLDP_SYS_NAME\n  value:           "
      "'OF|00:00:00:00:00:00:00:01'\n- field:           X_LLDP_CUSTOM1\n  "
      "value:           '0x26e1 0x0 "
      "4F467C2D32404F467C30303A30303A30303A30303A30303A30303A30303A3031'\n..."
      "\n");

  // Padded to 60 bytes
  testPacket(
      "00000000000200000000000188CC02070100112233445504030201020602004500000000"
      "000000000000000000000000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'00:00:00:00:00:02'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x88CC\n- field:           X_LLDP_CHASSIS_ID\n  value:           "
      "'chassis 001122334455'\n- field:           X_LLDP_PORT_ID\n  value:     "
      "      'port 0102'\n- field:           X_LLDP_TTL\n  value:           "
      "0x0045\n...\n");

  // Minimal LLDP (60 bytes)
  testPacket(
      "00000000000000000000000088CC02000400060200000000000000000000000000000000"
      "000000000000000000000000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'00:00:00:00:00:00'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:00'\n- field:           ETH_TYPE\n  value:           "
      "0x88CC\n- field:           X_LLDP_CHASSIS_ID\n  value:           "
      "'unknown'\n- field:           X_LLDP_PORT_ID\n  value:           "
      "'unknown'\n- field:           X_LLDP_TTL\n  value:           0x0000\n"
      "...\n");
}

TEST(matchpacket, ethernet_misaligned) {
  ByteList buf{HexToRawData("FFFFFFFFFFFF0000000000010806")};
  ASSERT_TRUE(IsPtrAligned(buf.data(), 8));

  MatchPacket match{ByteRange{buf.data(), buf.size()}};
  EXPECT_HEX("80000606:FFFFFFFFFFFF 80000806:000000000001 80000A02:0806",
             match.data(), match.size());
}

TEST(matchpacket, vlan_tag) {
  testPacket(
      "FFFFFFFFFFFF422B0B87AE5B810003E808060001080006040001422B0B87AE5B0A000001"
      "0000000000000A000002",
      "---\n- field:           ETH_DST\n  value:           "
      "'ff:ff:ff:ff:ff:ff'\n- field:           ETH_SRC\n  value:           "
      "'42:2b:0b:87:ae:5b'\n- field:           VLAN_VID\n  value:           "
      "0x13E8\n- field:           ETH_TYPE\n  value:           0x0806\n- "
      "field:           ARP_OP\n  value:           0x0001\n- field:           "
      "ARP_SPA\n  value:           10.0.0.1\n- field:           ARP_TPA\n  "
      "value:           10.0.0.2\n- field:           ARP_SHA\n  value:         "
      "  '42:2b:0b:87:ae:5b'\n- field:           ARP_THA\n  value:           "
      "'00:00:00:00:00:00'\n...\n");
}

TEST(matchpacket, icmpv6_nd_solicit) {
  testPacket(
      "3333FF100001BED925CBD7F0810007F886DD6000000000203AFFFC000000000000000000"
      "000000000001FF0200000000000000000001FF1000018700C4C400000000FC0000000000"
      "000000100010001000010101BED925CBD7F0",
      "---\n- field:           ETH_DST\n  value:           "
      "'33:33:ff:10:00:01'\n- field:           ETH_SRC\n  value:           "
      "'be:d9:25:cb:d7:f0'\n- field:           VLAN_VID\n  value:           "
      "0x17F8\n- field:           ETH_TYPE\n  value:           0x86DD\n- "
      "field:           NX_IP_TTL\n  value:           0xFF\n- field:           "
      "IP_PROTO\n  value:           0x3A\n- field:           IPV6_SRC\n  "
      "value:           'fc00::1'\n- field:           IPV6_DST\n  value:       "
      "    'ff02::1:ff10:1'\n- field:           ICMPV6_TYPE\n  value:          "
      " 0x87\n- field:           ICMPV6_CODE\n  value:           0x00\n- "
      "field:           IPV6_ND_TARGET\n  value:           "
      "'fc00::10:10:10:1'\n- field:           IPV6_ND_SLL\n  value:           "
      "'be:d9:25:cb:d7:f0'\n- field:           IPV6_EXTHDR\n  value:           "
      "0x0001\n...\n");
}

TEST(matchpacket, icmpv6_nd_advertise) {
  testPacket(
      "3333FF100001BED925CBD7F0810007F886DD6000000000203AFFFC000000000000000000"
      "000000000001FF0200000000000000000001FF1000018800C4C470000000FC0000000000"
      "000000100010001000010201BED925CBD7F0",
      "---\n- field:           ETH_DST\n  value:           "
      "'33:33:ff:10:00:01'\n- field:           ETH_SRC\n  value:           "
      "'be:d9:25:cb:d7:f0'\n- field:           VLAN_VID\n  value:           "
      "0x17F8\n- field:           ETH_TYPE\n  value:           0x86DD\n- "
      "field:           NX_IP_TTL\n  value:           0xFF\n- field:           "
      "IP_PROTO\n  value:           0x3A\n- field:           IPV6_SRC\n  "
      "value:           'fc00::1'\n- field:           IPV6_DST\n  value:       "
      "    'ff02::1:ff10:1'\n- field:           ICMPV6_TYPE\n  value:          "
      " 0x88\n- field:           ICMPV6_CODE\n  value:           0x00\n- "
      "field:           X_IPV6_ND_RES\n  value:           0x70000000\n- field: "
      "          IPV6_ND_TARGET\n  value:           'fc00::10:10:10:1'\n- "
      "field:           IPV6_ND_TLL\n  value:           'be:d9:25:cb:d7:f0'\n- "
      "field:           IPV6_EXTHDR\n  value:           0x0001\n...\n");
}

TEST(matchpacket, icmpv6_nd_router_advert) {
  testPacket(
      "3333FF1000012E296EEED14486DD6000000000083AFFFC00000000000000000000000000"
      "0001FF0200000000000000000001FF10000185007C9F01020304",
      "---\n- field:           ETH_DST\n  value:           "
      "'33:33:ff:10:00:01'\n- field:           ETH_SRC\n  value:           "
      "'2e:29:6e:ee:d1:44'\n- field:           ETH_TYPE\n  value:           "
      "0x86DD\n- field:           NX_IP_TTL\n  value:           0xFF\n- field: "
      "          IP_PROTO\n  value:           0x3A\n- field:           "
      "IPV6_SRC\n  value:           'fc00::1'\n- field:           IPV6_DST\n  "
      "value:           'ff02::1:ff10:1'\n- field:           ICMPV6_TYPE\n  "
      "value:           0x85\n- field:           ICMPV6_CODE\n  value:         "
      "  0x00\n- field:           IPV6_EXTHDR\n  value:           0x0001\n- "
      "field:           X_PKT_POS\n  value:           0x003A\n...\n");
}

TEST(matchpacket, icmpv4_echo_first_fragment) {
  testPacket(
      "0E00000000010AC2BB024296810000640800450005DC0518200040013AA70A0000010A64"
      "00FE080014572C2400059DA8FC590000000046D506000000000010111213141516171819"
      "1A1B1C1D1E1F20",
      "---\n- field:           ETH_DST\n  value:           "
      "'0e:00:00:00:00:01'\n- field:           ETH_SRC\n  value:           "
      "'0a:c2:bb:02:42:96'\n- field:           VLAN_VID\n  value:           "
      "0x1064\n- field:           ETH_TYPE\n  value:           0x0800\n- "
      "field:           IP_PROTO\n  value:           0x01\n- field:           "
      "IPV4_SRC\n  value:           10.0.0.1\n- field:           IPV4_DST\n  "
      "value:           10.100.0.254\n- field:           NX_IP_FRAG\n  value:  "
      "         0x01\n- field:           NX_IP_TTL\n  value:           0x40\n- "
      "field:           ICMPV4_TYPE\n  value:           0x08\n- field:         "
      "  ICMPV4_CODE\n  value:           0x00\n- field:           X_PKT_POS\n  "
      "value:           0x002A\n...\n");
}

TEST(matchpacket, icmpv4_echo_second_fragment) {
  testPacket(
      "0E00000000010AC2BB02429681000064080045000018051800B940015FB20A0000010A64"
      "00FEC0C1C2C3",
      "---\n- field:           ETH_DST\n  value:           "
      "'0e:00:00:00:00:01'\n- field:           ETH_SRC\n  value:           "
      "'0a:c2:bb:02:42:96'\n- field:           VLAN_VID\n  value:           "
      "0x1064\n- field:           ETH_TYPE\n  value:           0x0800\n- "
      "field:           IP_PROTO\n  value:           0x01\n- field:           "
      "IPV4_SRC\n  value:           10.0.0.1\n- field:           IPV4_DST\n  "
      "value:           10.100.0.254\n- field:           NX_IP_FRAG\n  value:  "
      "         0x03\n- field:           NX_IP_TTL\n  value:           0x40\n- "
      "field:           X_PKT_POS\n  value:           0x0026\n...\n");
}

TEST(matchpacket, icmpv4_padded) {
  // icmpv4
  testPacket(
      "00000000000200000000000108004500001D000000004001F78CC0A80101C0A801020000"
      "F6F901020304050000000000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'00:00:00:00:00:02'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_PROTO\n  value:           0x01\n- field:  "
      "         IPV4_SRC\n  value:           192.168.1.1\n- field:           "
      "IPV4_DST\n  value:           192.168.1.2\n- field:           "
      "NX_IP_TTL\n  value:           0x40\n- field:           ICMPV4_TYPE\n  "
      "value:           0x00\n- field:           ICMPV4_CODE\n  value:         "
      "  0x00\n- field:           X_PKT_POS\n  value:           0x0026\n...\n");

  // icmpv4_2
  testPacket(
      "00000000000200000000000108004500001D000000004001F98BC0A80003C0A800010800"
      "7FCD78320000000000000000000000000000000000000000",
      "---\n- field:           ETH_DST\n  value:           "
      "'00:00:00:00:00:02'\n- field:           ETH_SRC\n  value:           "
      "'00:00:00:00:00:01'\n- field:           ETH_TYPE\n  value:           "
      "0x0800\n- field:           IP_PROTO\n  value:           0x01\n- field:  "
      "         IPV4_SRC\n  value:           192.168.0.3\n- field:           "
      "IPV4_DST\n  value:           192.168.0.1\n- field:           "
      "NX_IP_TTL\n  value:           0x40\n- field:           ICMPV4_TYPE\n  "
      "value:           0x08\n- field:           ICMPV4_CODE\n  value:         "
      "  0x00\n- field:           X_PKT_POS\n  value:           0x0026\n...\n");
}
