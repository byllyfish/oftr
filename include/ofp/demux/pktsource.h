#ifndef OFP_DEMUX_PKTSOURCE_H_
#define OFP_DEMUX_PKTSOURCE_H_

#include "ofp/timestamp.h"
#include "ofp/byterange.h"
#include <pcap/pcap.h>

namespace ofp {
namespace demux {

/// \brief A concrete class that adapts the libpcap API.
/// 
/// PktSource can capture live packets from the network. PktSource can also 
/// read packets from a `pcap` file offline.
/// 
/// To capture TCP packets from the "en0" interface.
/// 
///     PktSource src;
///     if (src.openDevice("en0", "tcp")) {
///         src.runLoop([](Timestamp ts, unsigned len, ByteRange data, void *context) {
///             log::debug("pkt", ts, len, data);
///         });
///     }
///     
/// To read packets offline from the file "data.pcap":
/// 
///     PktSource src;
///     if (src.openFile("data.pcap")) {
///         src.runLoop([](Timestamp ts, unsigned len, ByteRange data, void *context) {
///             log::debug("pkt", ts, len, data);
///         });
///     }

OFP_BEGIN_IGNORE_PADDING

class PktSource {
public:

    enum {
        DEFAULT_SNAPLEN = 65535,                    // 65535 bytes
        DEFAULT_PROMISC = 1,                        // TRUE
        DEFAULT_TIMEOUT = 1000,                     // 1000 msec
        DEFAULT_BUFFER_SIZE = 5 * 1024 * 1024,      // 5 MB
    };

    using PktCallback = void(*)(Timestamp, ByteRange, unsigned, void *);

    PktSource() = default;
    ~PktSource() { close(); }

    bool openDevice(const std::string &device, const std::string &filter);
    bool openFile(const std::string &file, const std::string &filter);
    void close();

    bool runLoop(int count, PktCallback callback, void *context = nullptr);

    const std::string error() const { return error_; }

private:
    pcap_t *pcap_ = nullptr;
    std::string error_;
    UInt32 nanosec_factor_ = 0;

    bool create(const std::string &device);
    bool activate();
    bool setFilter(const std::string &filter);
    void setTimestampPrecision();
    void setError(const char *func, const std::string &device, const char *result);
};

OFP_END_IGNORE_PADDING

}  // namespace pcap
}  // namespace ofp

#endif // OFP_DEMUX_PKTSOURCE_H_
