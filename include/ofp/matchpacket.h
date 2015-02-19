#ifndef OFP_MATCHPACKET_H_
#define OFP_MATCHPACKET_H_

#include "ofp/matchbuilder.h"

namespace ofp {

class MatchPacket {
public:
    enum DecodeResult {
      kFullDecode = 0,
      kPartialDecode = 1,
      kFailedDecode = 2,
    };

    explicit MatchPacket(const ByteRange &data);

    const UInt8 *data() const { return match_.data(); }
    size_t size() const { return match_.size(); }

private:
    MatchBuilder match_;
    DecodeResult result_;

    void decodeEthernet(const UInt8 *pkt, size_t length);
    void decodeARP(const UInt8 *pkt, size_t length);
    void decodeIPv4(const UInt8 *pkt, size_t length);
    void decodeIPv4_NextHdr(const UInt8 *pkt, size_t length, UInt8 nextHdr);
    void decodeIPv6(const UInt8 *pkt, size_t length);
    void decodeIPv6_NextHdr(const UInt8 *pkt, size_t length, UInt8 nextHdr);
    void decodeTCP(const UInt8 *pkt, size_t length);
    void decodeUDP(const UInt8 *pkt, size_t length);
    void decodeICMP(const UInt8 *pkt, size_t length, bool version6);
    void decodeLLDP(const UInt8 *pkt, size_t length);

    UInt8 nextIPv6ExtHdr(const UInt8 *&pkt, size_t &length);
};

}  // namespace ofp

#endif // OFP_MATCHPACKET_H_
