#include "ofp/unittest.h"
#include "ofp/demux/messagesource.h"
#include "ofp/demux/pktsource.h"
#include "ofp/message.h"
#include "ofp/yaml/decoder.h"

using namespace ofp;
using demux::PktSource;
using demux::MessageSource;


static void callback(Message *message) {
    message->transmogrify();

    ofp::yaml::Decoder decoder{message, false, true};

    if (decoder.error().empty())
        log::debug("callback\n", decoder.result());
    else
        log::debug("callback error:", decoder.error());
}

TEST(messagesource, test) {
    PktSource pkt;
    MessageSource msg(callback);

    if (pkt.openFile("/Users/bfish/Downloads/cap_single,3-ovsk,protocols=OpenFlow13-remote,port=6653.pcap", "tcp")) {
        pkt.runLoop(0, [](Timestamp ts, ByteRange data, unsigned len, void *context) {
            MessageSource *src = reinterpret_cast<MessageSource *>(context);
            src->submitPacket(ts, data);
        }, &msg);
    } else {
        log::debug("PktSource error:", pkt.error());
    }
}
