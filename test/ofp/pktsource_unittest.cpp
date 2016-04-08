#include "ofp/unittest.h"
#include "ofp/demux/pktsource.h"
#include "ofp/timestamp.h"

using namespace ofp;


TEST(pktsource, test) {
    demux::PktSource src;

#if 0
    if (src.openDevice("en0", "tcp")) {
        src.runLoop(0, [](Timestamp ts, ByteRange data, unsigned len, void *context) {
            log::debug("pcap_handler:", ts, len, data);
        });
    } else {
        log::debug("PktSource error:", src.error());
    }
#endif //0


    if (src.openFile("/Users/bfish/Downloads/cap_single,3-default-default.pcap", "tcp")) {
        src.runLoop(0, [](Timestamp ts, ByteRange data, unsigned len, void *context) {
            log::debug("pcap_handler:", ts, len, data);
        });
    } else {
        log::debug("PktSource error:", src.error());
    }
}
