// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MATCHPACKET_H_
#define OFP_MATCHPACKET_H_

#include "ofp/matchbuilder.h"

namespace ofp {

class MatchPacket {
 public:
  explicit MatchPacket(const ByteRange &data, bool warnMisaligned = true);

  OXMIterator begin() const { return match_.begin(); }
  OXMIterator end() const { return match_.end(); }

  const UInt8 *data() const { return match_.data(); }
  size_t size() const { return match_.size(); }

 private:
  MatchBuilder match_;
  size_t offset_ = 0;

  void decode(const UInt8 *pkt, size_t length);
  void decodeEthernet(const UInt8 *pkt, size_t length);
  void decodeARP(const UInt8 *pkt, size_t length);
  void decodeIPv4(const UInt8 *pkt, size_t length);
  void decodeIPv4_NextHdr(const UInt8 *pkt, size_t length, UInt8 nextHdr);
  void decodeIPv6(const UInt8 *pkt, size_t length);
  void decodeIPv6_NextHdr(const UInt8 *pkt, size_t length, UInt8 nextHdr);
  void decodeTCP(const UInt8 *pkt, size_t length);
  void decodeUDP(const UInt8 *pkt, size_t length);
  void decodeICMPv4(const UInt8 *pkt, size_t length);
  void decodeICMPv6(const UInt8 *pkt, size_t length);
  void decodeLLDP(const UInt8 *pkt, size_t length);

  UInt8 nextIPv6ExtHdr(UInt8 currHdr, const UInt8 *&pkt, size_t &length,
                       UInt32 &flags);
  void countIPv6ExtHdr(UInt32 &flags, UInt32 hdr, UInt32 precedingHdrs);
};

}  // namespace ofp

#endif  // OFP_MATCHPACKET_H_
