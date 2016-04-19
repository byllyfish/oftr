// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/messagesource.h"
#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/messageinfo.h"
#include "ofp/pkt.h"

using namespace ofp;
using namespace ofp::demux;

void MessageSource::submitPacket(Timestamp ts, ByteRange capture) {
  log::debug("submitPacket", capture);

  const UInt8 *data = capture.data();
  size_t length = capture.size();

  auto eth = pkt::Ethernet::cast(data, length);
  if (!eth) {
    log::warning("MessageSource: No ethernet header");
    return;
  }

  ts_ = ts;
  data += sizeof(pkt::Ethernet);
  length -= sizeof(pkt::Ethernet);

  UInt16 ethType = eth->type;
  if (ethType == DATALINK_IPV4) {
    submitIPv4(data, length);
  } else if (ethType == DATALINK_IPV6) {
    submitIPv6(data, length);
  } else {
    log::debug("MessageSource: Skip ethernet type", ethType);
  }
}

void MessageSource::close() {}

void MessageSource::submitIPv4(const UInt8 *data, size_t length) {
  auto ip = pkt::IPv4Hdr::cast(data, length);
  if (!ip) {
    log::warning("MessageSource: No IPv4 header");
    return;
  }

  UInt8 vers = ip->ver >> 4;
  UInt8 ihl = ip->ver & 0x0F;

  if (vers != pkt::kIPv4Version) {
    log::warning("MessageSource: Unexpected IPv4 version", vers);
    return;
  }

  if (ihl < 5) {
    log::warning("MessageSource: Unexpected IPv4 header length", ihl);
    return;
  }

  unsigned hdrLen = ihl * 4;
  if (hdrLen > length) {
    log::warning("MessageSource: IPv4 Header too long", hdrLen);
    return;
  }

  src_.setAddress(ip->src);
  dst_.setAddress(ip->dst);
  len_ = ip->length;

  if (len_ < length) {
    log::warning("MessageSource: IPv4 packet is padded", length - len_);
    length = len_;
  }

  data += hdrLen;
  length -= hdrLen;

  if (ip->proto == PROTOCOL_TCP) {
    submitTCP(data, length);
  } else if (ip->proto == PROTOCOL_UDP) {
    submitUDP(data, length);
  } else {
    log::debug("MessageSource: Ignored IPv4 proto", ip->proto);
  }
}

void MessageSource::submitIPv6(const UInt8 *data, size_t length) {
  log::error("submitIPv6: IPv6 not implemented");
}

void MessageSource::submitTCP(const UInt8 *data, size_t length) {
  auto tcp = pkt::TCPHdr::cast(data, length);
  if (!tcp) {
    log::warning("MessageSource: No TCP header");
    return;
  }

  flags_ = tcp->flags;

  unsigned tcpHdrLen = (flags_ >> 12) * 4;
  if (tcpHdrLen < sizeof(pkt::TCPHdr) || tcpHdrLen > length) {
    log::warning("MessageSource: TCP header invalid data offset");
    return;
  }

  src_.setPort(tcp->srcPort);
  dst_.setPort(tcp->dstPort);
  seqNum_ = tcp->seqNum;

  data += tcpHdrLen;
  length -= tcpHdrLen;

  if (flags_ & TCP_SYN) {
    ++seqNum_;
    if (length > 0) {
      log::warning("MessageSource: TCP SYN unexpected data", length);
      return;
    }
  }

  auto read = flows_.receive(ts_, src_, dst_, seqNum_ + length, {data, length},
                             UInt8_narrow_cast(flags_));
  if (read.size() > 0) {
    size_t n = submitPayload(read.data(), read.size());
    log::debug("submitTCP: consume", n, "bytes");
    read.consume(n);
  }
}

void MessageSource::submitUDP(const UInt8 *data, size_t length) {}

size_t MessageSource::submitPayload(const UInt8 *data, size_t length) {
  log::debug("submitPayload", src_, dst_, len_, ByteRange{data, length});

  // Deliver completed messages in the payload buffer.
  size_t remaining = length;

  while (remaining >= sizeof(Header)) {
    UInt16 msgLen = Big16_copy(data + 2);

    if (msgLen < sizeof(Header)) {
      log::warning("submitPayload: msg length < 8 bytes", msgLen);
      msgLen = 8;
    }

    if (remaining >= msgLen) {
      deliverMessage(data, msgLen);
      data += msgLen;
      remaining -= msgLen;
    } else {
      break;
    }
  }

  assert(length >= remaining);

  return length - remaining;
}

void MessageSource::deliverMessage(const UInt8 *data, size_t length) {
  log::info("deliverMessage:", seqNum_, ByteRange(data, length));

  if (length < 8) {
    log::warning("deliverMessage: Message too small");
    return;
  }

  MessageInfo info{1, src_, dst_};
  Message message{data, length};
  message.setInfo(&info);
  message.setTime(ts_);

  if (callback_)
    callback_(&message, context_);
}
