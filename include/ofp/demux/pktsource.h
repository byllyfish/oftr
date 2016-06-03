// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_PKTSOURCE_H_
#define OFP_DEMUX_PKTSOURCE_H_

#include <pcap/pcap.h>
#include "ofp/byterange.h"
#include "ofp/timestamp.h"

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
///         src.runLoop([](Timestamp ts, ByteRange data, unsigned len, void
///         *context) {
///             log::debug("pkt", ts, len, data);
///         });
///     }
///
/// To read packets offline from the file "data.pcap":
///
///     PktSource src;
///     if (src.openFile("data.pcap", "tcp")) {
///         src.runLoop([](Timestamp ts, ByteRange data, unsigned len, void
///         *context) {
///             log::debug("pkt", ts, len, data);
///         });
///     }

OFP_BEGIN_IGNORE_PADDING

class PktSource {
 public:
  enum {
    DEFAULT_SNAPLEN = 65535,                // 65535 bytes
    DEFAULT_PROMISC = 1,                    // TRUE
    DEFAULT_TIMEOUT = 1000,                 // 1000 msec
    DEFAULT_BUFFER_SIZE = 5 * 1024 * 1024,  // 5 MB
  };

  enum Encapsulation {
    ENCAP_UNSUPPORTED = 0,
    ENCAP_ETHERNET = 1,
    ENCAP_IP = 2,
  };

  using PktCallback = void (*)(Timestamp, ByteRange, unsigned, void *);

  PktSource() = default;
  ~PktSource() { close(); }

  Encapsulation encapsulation() const { return encap_; }
  std::string datalink() const;

  bool openDevice(const std::string &device, const std::string &filter);
  bool openFile(const std::string &file, const std::string &filter);
  void close();

  bool runLoop(PktCallback callback, void *context = nullptr);

  const std::string error() const { return error_; }

 private:
  pcap_t *pcap_ = nullptr;
  std::string error_;
  UInt32 nanosec_factor_ = 1000;
  Encapsulation encap_ = ENCAP_UNSUPPORTED;
  UInt32 frameSkip_ = 0;
  int datalink_ = -1;

  bool create(const std::string &device);
  bool openOffline(const std::string &path);
  bool activate();
  bool checkDatalink();
  bool setFilter(const std::string &filter);
  void setError(const char *func, const std::string &device,
                const char *result);

  static Encapsulation lookupEncapsulation(int datalink, UInt32 *frameSkip);
};

OFP_END_IGNORE_PADDING

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_PKTSOURCE_H_
