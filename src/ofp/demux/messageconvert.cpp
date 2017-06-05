#include "ofp/demux/messageconvert.h"
#include "ofp/demux/pktsource.h"
#include "ofp/memorychannel.h"
#include "ofp/message.h"
#include "ofp/packetin.h"
#include "ofp/pkt.h"

using namespace ofp;
using namespace ofp::demux;

static void sEthernetCallback(Timestamp ts, ByteRange data, unsigned len,
                              void *context) {
  MessageConvert *src = reinterpret_cast<MessageConvert *>(context);
  src->submitEthernet(ts, data, len);
}

static void sIPCallback(Timestamp ts, ByteRange data, unsigned len,
                        void *context) {
  if (data.empty())
    return;

  UInt8 vers = (data[0] >> 4);
  UInt16 ethType = 0;
  if (vers == pkt::kIPv4Version) {
    ethType = DATALINK_IPV4;
  } else if (vers == pkt::kIPv6Version) {
    ethType = DATALINK_IPV6;
  } else {
    log_warning("Unknown IP version:", static_cast<unsigned>(vers));
    return;
  }

  pkt::Ethernet enet;
  enet.type = ethType;
  len += sizeof(pkt::Ethernet);

  ByteList enetFrame;
  enetFrame.add(&enet, sizeof(enet));
  enetFrame.add(data.data(), data.size());
  sEthernetCallback(ts, enetFrame, len, context);
}

void MessageConvert::runLoop(PktSource *pcap) {
  if (pcap->encapsulation() == PktSource::ENCAP_ETHERNET) {
    pcap->runLoop(sEthernetCallback, this);
  } else {
    pcap->runLoop(sIPCallback, this);
  }
}

void MessageConvert::submitEthernet(Timestamp ts, ByteRange capture,
                                    unsigned len) {
  PacketInBuilder pktBuilder;
  pktBuilder.setTotalLen(UInt16_narrow_cast(len));
  pktBuilder.setEnetFrame(capture);

  MemoryChannel channel{OFP_VERSION_4};
  pktBuilder.send(&channel);

  Message message{channel.data(), channel.size()};

  if (callback_)
    callback_(&message, context_);
}
