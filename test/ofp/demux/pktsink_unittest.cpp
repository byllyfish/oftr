#include "ofp/unittest.h"
#include "ofp/demux/pktsink.h"

using namespace ofp;

TEST(pktsink, test) {
#if 0
    demux::PktSink sink;
    ByteList data{HexToRawData("0000000000010000000000020800")};
    Timestamp now = Timestamp::now();

    if (sink.openFile("/tmp/pktsink_test.pcap")) {
        for (unsigned i = 0; i < 10; ++i) {
            Timestamp ts{now.seconds(), now.nanoseconds() + i + 1000*i};
            sink.write(ts, data.toRange(), 1000);
        }
    }
#endif //0
}
