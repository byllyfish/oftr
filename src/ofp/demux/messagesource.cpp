// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/messagesource.h"
#include <sys/stat.h>  // mkdir for debugWrite
#include <fstream>     // for debugWrite
#include "ofp/demux/pktsource.h"
#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/messageinfo.h"
#include "ofp/pkt.h"

using namespace ofp;
using namespace ofp::demux;

const UInt16 DATALINK_8021Q = 0x8100;
const size_t k8021QHeaderSize = 4;

static void sEthernetCallback(Timestamp ts, ByteRange data, unsigned len,
                              void *context) {
  MessageSource *src = reinterpret_cast<MessageSource *>(context);
  src->submitEthernet(ts, data);
}

static void sIPCallback(Timestamp ts, ByteRange data, unsigned len,
                        void *context) {
  MessageSource *src = reinterpret_cast<MessageSource *>(context);
  src->submitIP(ts, data);
}

void MessageSource::runLoop(PktSource *pcap) {
  PktSource::PktCallback callback;

  if (pcap->encapsulation() == PktSource::ENCAP_ETHERNET) {
    callback = sEthernetCallback;
  } else {
    callback = sIPCallback;
  }

  pcap->runLoop(callback, this);
  finish();
}

void MessageSource::submitEthernet(Timestamp ts, ByteRange capture) {
  const UInt8 *data = capture.data();
  size_t length = capture.size();

  ts_ = ts;
  submitEthernet(data, length);
}

void MessageSource::submitIP(Timestamp ts, ByteRange capture) {
  const UInt8 *data = capture.data();
  size_t length = capture.size();

  ts_ = ts;

  if (length > 0) {
    UInt8 vers = (data[0] >> 4);
    if (vers == pkt::kIPv4Version) {
      submitIPv4(data, length);
    } else if (vers == pkt::kIPv6Version) {
      submitIPv6(data, length);
    } else {
      log_debug("MessageSource: Unknown IP version", vers);
    }
  }
}

void MessageSource::finish() {
  log_debug("MessageSource::finish\n", flows_.toString());

  if (hasOutputDir()) {
    flows_.finish(
        [this](const IPv6Endpoint &src, const IPv6Endpoint &dst,
               const FlowData &flow) {
          outputWrite(src, dst, flow, flow.size());
        },
        maxMissingBytes_);
  }
  log_error("MessageSource: ", flows_.stats());
  flows_.clear();
}

void MessageSource::submitEthernet(const UInt8 *data, size_t length) {
  auto eth = pkt::Ethernet::cast(data, length);
  if (!eth) {
    log_warning("MessageSource: No ethernet header");
    return;
  }

  data += sizeof(pkt::Ethernet);
  length -= sizeof(pkt::Ethernet);

  UInt16 ethType = eth->type;

  // Ignore 802.1Q header and vlan.
  if (ethType == DATALINK_8021Q && length >= k8021QHeaderSize) {
    ethType = *Big16_cast(data + 2);
    data += k8021QHeaderSize;
    length -= k8021QHeaderSize;
  }

  if (ethType == DATALINK_IPV4) {
    submitIPv4(data, length);
  } else if (ethType == DATALINK_IPV6) {
    submitIPv6(data, length);
  } else {
    log_debug("MessageSource: Skip ethernet type", ethType);
  }
}

void MessageSource::submitIPv4(const UInt8 *data, size_t length) {
  auto ip = pkt::IPv4Hdr::cast(data, length);
  if (!ip) {
    log_warning("MessageSource: No IPv4 header");
    return;
  }

  UInt8 vers = ip->ver >> 4;
  UInt8 ihl = ip->ver & 0x0F;

  if (vers != pkt::kIPv4Version) {
    log_warning("MessageSource: Unexpected IPv4 version", vers);
    return;
  }

  if (ihl < 5) {
    log_warning("MessageSource: Unexpected IPv4 header length", ihl);
    return;
  }

  UInt32 len = ip->length;
  if (len < length) {
    // IPv4 packet is padded; shorten length based on IP header.
    length = len;
  }

  UInt32 hdrLen = ihl * 4;
  if (hdrLen > length) {
    log_warning("MessageSource: IPv4 Header too long", hdrLen);
    return;
  }

  // Check for fragmented IPv4 packet.
  UInt16 frag = ip->frag & (pkt::IPv4_MoreFragMask | pkt::IPv4_FragOffsetMask);
  if (frag) {
    log_warning("MessageSource: IPv4 packet is fragment");
    return;
  }

  src_.setAddress(ip->src);
  dst_.setAddress(ip->dst);

  assert(length >= hdrLen);

  data += hdrLen;
  length -= hdrLen;

  if (ip->proto == PROTOCOL_TCP) {
    submitTCP(data, length);
  } else if (ip->proto == PROTOCOL_UDP) {
    submitUDP(data, length);
  } else {
    log_debug("MessageSource: Ignored IPv4 proto", ip->proto);
  }
}

void MessageSource::submitIPv6(const UInt8 *data, size_t length) {
  auto ip = pkt::IPv6Hdr::cast(data, length);
  if (!ip) {
    log_warning("MessageSource: No IPv6 header");
    return;
  }

  UInt32 verClassLabel = ip->verClassLabel;
  UInt8 vers = verClassLabel >> 28;
  if (vers != pkt::kIPv6Version) {
    log_warning("MessageSource: Unexpected IPv6 version", vers);
    return;
  }

  UInt32 len = ip->payloadLength;
  src_.setAddress(ip->src);
  dst_.setAddress(ip->dst);

  data += sizeof(pkt::IPv6Hdr);
  length -= sizeof(pkt::IPv6Hdr);

  if (len < length) {
    // IPv6 packet is padded; shorten length based on IPv6 header.
    length = len;
  }

  if (ip->nextHeader == PROTOCOL_TCP) {
    submitTCP(data, length);
  } else if (ip->nextHeader == PROTOCOL_UDP) {
    submitUDP(data, length);
  } else {
    log_debug("MessageSource: Ignored IPv6 proto", ip->nextHeader);
  }
}

void MessageSource::submitTCP(const UInt8 *data, size_t length) {
  auto tcp = pkt::TCPHdr::cast(data, length);
  if (!tcp) {
    log_warning("MessageSource: No TCP header");
    return;
  }

  flags_ = tcp->flags;

  unsigned tcpHdrLen = (flags_ >> 12) * 4;
  if (tcpHdrLen < sizeof(pkt::TCPHdr) || tcpHdrLen > length) {
    log_warning("MessageSource: TCP header invalid data offset");
    return;
  }

  src_.setPort(tcp->srcPort);
  dst_.setPort(tcp->dstPort);
  seq_ = tcp->seqNum;

  data += tcpHdrLen;
  length -= tcpHdrLen;

  auto flow = flows_.receive(ts_, src_, dst_, seq_, {data, length},
                             UInt8_narrow_cast(flags_));

  if (flow.size() > 0) {
    size_t n = submitPayload(flow.data(), flow.size(), flow.sessionID());
    if (flow.final() && n != flow.size()) {
      log_debug("MessageSource: TCP done before full message received",
                flow.sessionID());
      // Make sure we consume all of the remaining data.
      n = flow.size();
    }
    if (hasOutputDir()) {
      outputWrite(src_, dst_, flow, n);
    }
    flow.consume(n);
  }
}

void MessageSource::submitUDP(const UInt8 *data, size_t length) {}

size_t MessageSource::submitPayload(const UInt8 *data, size_t length,
                                    UInt64 sessionID) {
  // Ignore the payload; this option is useful for debugging only.
  if (skipPayload_) {
    return length;
  }

  // Deliver completed messages in the payload buffer.
  size_t remaining = length;

  while (remaining >= sizeof(Header)) {
    UInt16 msgLen = Big16_copy(data + 2);

    if (msgLen >= sizeof(Header) && remaining >= msgLen) {
      deliverMessage(data, msgLen, sessionID);
      data += msgLen;
      remaining -= msgLen;
    } else if (msgLen < sizeof(Header)) {
      log_warning("submitPayload: msgLen smaller than 8 bytes:", msgLen,
                  "(skip 8 bytes)");
      data += sizeof(Header);
      remaining -= sizeof(Header);
    } else {
      break;
    }
  }

  assert(length >= remaining);

  return length - remaining;
}

void MessageSource::deliverMessage(const UInt8 *data, size_t length,
                                   UInt64 sessionID) {
  log_debug("deliverMessage:", sessionID, ByteRange(data, length));
  assert(length >= sizeof(Header));

  MessageInfo info{sessionID, src_, dst_};
  Message message{data, length};
  message.setInfo(&info);
  message.setTime(ts_);

  if (callback_)
    callback_(&message, context_);
}

void MessageSource::outputWrite(const IPv6Endpoint &src,
                                const IPv6Endpoint &dst, const FlowData &flow,
                                size_t n) {
  assert(hasOutputDir());

  // If there's no data, don't create any files.
  if (n == 0)
    return;

  // Construct filename "$outputDir/_tcp-$session-$src-$dst"
  std::ostringstream oss;
  oss << outputDir_ << "/_tcp-" << flow.sessionID() << '-' << src << '-' << dst;
  auto filename = oss.str();

  // Write flow to a file.
  std::ofstream out{filename, std::ios::out | std::ios::app | std::ios::binary};
  if (out) {
    log_debug("outputWrite", filename, ByteRange{flow.data(), n});
    out.write(reinterpret_cast<const char *>(flow.data()), Signed_cast(n));
  } else {
    log_error("MessageSource: Unable to open output file", filename);
  }
}
