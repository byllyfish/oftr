// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_PKTSINK_H_
#define OFP_DEMUX_PKTSINK_H_

#include <pcap/pcap.h>

#include "ofp/byterange.h"
#include "ofp/timestamp.h"

namespace ofp {
namespace demux {

/// \brief A concrete class that writes to a ".pcap" file.
///
/// Adapts the libpcap API for dumping packets.
///
/// To write packets to a file "data.pcap":
///
///   PktSink sink;
///   if (sink.openFile("data.pcap")) {
///     ByteRange data = {"abcdefabcdef14", 14};
///     sink.write(data, 1500);
///   } else {
///     llvm::errs() << sink.error();
///   }

OFP_BEGIN_IGNORE_PADDING

class PktSink {
 public:
  PktSink() = default;
  ~PktSink() { close(); }

  bool openFile(const std::string &file);
  void close();

  void write(const Timestamp &ts, const ByteRange &captureData, UInt32 length);

  const std::string error() const { return error_; }

 private:
  pcap_t *pcap_ = nullptr;
  pcap_dumper_t *dump_ = nullptr;
  std::string error_;
  bool haveNanosec_ = false;

  void setError(const char *func, const char *msg);
};

OFP_END_IGNORE_PADDING

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_PKTSINK_H_
