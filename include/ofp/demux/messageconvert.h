#ifndef OFP_DEMUX_MESSAGECONVERT_H_
#define OFP_DEMUX_MESSAGECONVERT_H_

#include "ofp/timestamp.h"
#include "ofp/byterange.h"

namespace ofp {

class Message;

namespace demux {

class PktSource;

/// Convert each captured packet to a PacketIn message.

class MessageConvert {
public:
    using MessageCallback = void (*)(Message *, void *);

    MessageConvert(MessageCallback callback, void *context) : callback_{callback}, context_{context} {}

    void runLoop(PktSource *pcap);

    void submitEthernet(Timestamp ts, ByteRange capture, unsigned len);

private:
    MessageCallback callback_;
    void *context_ = nullptr;
};

}  // namespace demux
}  // namespace ofp

#endif // OFP_DEMUX_MESSAGECONVERT_H_
