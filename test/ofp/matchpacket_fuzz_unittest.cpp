#include "ofp/unittest.h"
#include "ofp/matchpacket.h"
#include "ofp/matchpacketbuilder.h"

using namespace ofp;

static void fuzzPacket(const char *hex, size_t start = 0) {
  ByteList buf{HexToRawData(hex)};
  ByteList result;

  EXPECT_TRUE(buf.size() >= 14);

  // Insert two zero bytes at the beginning. MatchPacket expects packet data
  // to be aligned at 2 bytes past the 8-byte alignment.
  buf.insertZeros(buf.begin(), 2);

  // Mutate bytes.
  for (size_t i = start + 2; i < buf.size(); ++i) {
    UInt8 saved = buf[i];
    buf[i] = 0xFF;

    ByteRange pkt{buf.data() + 2, buf.size() - 2};
    MatchPacket match{pkt};
    EXPECT_TRUE(match.size() > 0);

    MatchPacketBuilder builder{match.toRange()};

    size_t offset = match.toRange().get<X_PKT_POS>();
    if (offset == 0) {
        offset = buf.size() - 2;
    }

    ByteRange remaining = SafeByteRange(buf.data() + 2, buf.size() - 2, offset);
    result.clear();
    builder.build(&result, remaining);

    EXPECT_TRUE(result.size() > 0);

    buf[i] = saved;
  }
}


TEST(matchpacket_fuzz, ethernet) {
    // Ethernet
    fuzzPacket("1111111111112222222222223333");
    fuzzPacket("111111111111222222222222333344");

    // Arp
    fuzzPacket("111111111111222222222222080600010800060400010000000000010A0000010000000000000A000002");

    // IPv4
    fuzzPacket("111111111111222222222222080045334444555500007706888899999999AAAAAAAA");

#if 0
    // TCPv4
    fuzzPacket("111111111111222222222222080045334444555500007706888899999999AAAAAAAABBBBCCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFFFFFF");

    // UDPv4
    fuzzPacket("111111111111222222222222080045334444555500007711888899999999AAAAAAAABBBBCCCCDDDDEEEE");

    // ICMPv4
    fuzzPacket("111111111111222222222222080045334444555500007701888899999999AAAAAAAABBCCDDDDDDDD");

    // ICMPv6
    fuzzPacket("3333FF718C4C32FA25D4234186DD6000000000183AFF00000000000000000000000000000000FF0200000000000000000001FF718C4C87009A9E00000000FE80000000000000FC0CCDFFFE718C4C");

    // TCPv6
    fuzzPacket("11111111111122222222222286DD63344444555511667777777777777777777777777777777788888888888888888888888888888888BBBBCCCCDDDDDDDDDDDDDDDDEEEEFFFFFFFFFFFF");

    // UDPv6
    fuzzPacket("11111111111122222222222286DD63344444555511667777777777777777777777777777777788888888888888888888888888888888BBBBCCCCDDDDEEEE");

    // LLDP
    fuzzPacket("0180C200000E00000000000188CC0207040000000000010403072D32060200780A1A4F467C30303A30303A30303A30303A30303A30303A30303A3031FE240026E1004F467C2D32404F467C30303A30303A30303A30303A30303A30303A30303A30310000");
#endif //0

}
