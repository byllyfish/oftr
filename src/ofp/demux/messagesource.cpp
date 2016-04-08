#include "ofp/demux/messagesource.h"
#include "ofp/pkt.h"
#include "ofp/header.h"
#include "ofp/message.h"
#include "ofp/messageinfo.h"

using namespace ofp;
using namespace ofp::demux;


void MessageSource::submitPacket(Timestamp ts, ByteRange capture) {
    //log::debug("submitPacket", capture);

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

void MessageSource::close() {

}


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

    data += hdrLen;
    length -= hdrLen;

    src_ = ip->src;
    dst_ = ip->dst;
    len_ = ip->length;

    if (ip->proto == PROTOCOL_TCP) {
        submitTCP(data, length);
    } else if (ip->proto == PROTOCOL_UDP) {
        submitUDP(data, length);
    } else {
        log::debug("MessageSource: Ignored IPv4 proto", ip->proto);
    }
}

void MessageSource::submitIPv6(const UInt8 *data, size_t length) {

}

void MessageSource::submitTCP(const UInt8 *data, size_t length) {
    auto tcp = pkt::TCPHdr::cast(data, length);
    if (!tcp) {
        log::warning("MessageSource: No TCP header");
        return;
    }

    srcPort_ = tcp->srcPort;
    dstPort_ = tcp->dstPort;
    flags_ = tcp->flags;
    seqNum_ = tcp->seqNum;
    ackNum_ = tcp->ackNum;

    unsigned tcpHdrLen = (flags_ >> 12) * 4;
    if (tcpHdrLen < sizeof(pkt::TCPHdr) || tcpHdrLen > length) {
        log::warning("MessageSource: TCP header invalid data offset");
        return;
    }

    data += tcpHdrLen;
    length -= tcpHdrLen;

    submitPayload(data, length);
}

void MessageSource::submitUDP(const UInt8 *data, size_t length) {

}



void MessageSource::submitPayload(const UInt8 *data, size_t length) {
    //log::debug("submitPayload", src_, srcPort_, dst_, dstPort_, flags_, len_, ByteRange{data, length});

    // Deliver completed messages in the payload buffer.
    UInt16 msgLen = 0;
    while (length >= sizeof(Header)) {
        msgLen = Big16_copy(data + 2);
        if (length >= msgLen) {
            deliverMessage(data, msgLen);
            data += msgLen;
            length -= msgLen;
        } else {
            break;
        }
    }

    if (length != 0) {
        log::warning("submitPayload: Incomplete message", msgLen, length);
    }
}

void MessageSource::deliverMessage(const UInt8 *data, size_t length) {
    log::info("deliverMessage:", seqNum_, ackNum_, ByteRange(data, length));

    MessageInfo info{1, {src_, srcPort_}, {dst_, dstPort_}};
    Message message{data, length};
    message.setInfo(&info);
    message.setTime(ts_);

    if (callback_)
        callback_(&message);
}
