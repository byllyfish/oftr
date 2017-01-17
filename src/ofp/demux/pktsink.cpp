// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/pktsink.h"
#include <sstream>

using namespace ofp;
using namespace ofp::demux;

const int DEFAULT_LINK_TYPE = DLT_EN10MB;
const int DEFAULT_SNAP_LEN = 0x40000;

bool PktSink::openFile(const std::string &file) {
  if (pcap_)
    close();

#if HAVE_PCAP_OPEN_DEAD_WITH_TSTAMP_PRECISION
  // Use the API that supports nanosecond precision if it't available.
  pcap_ = pcap_open_dead_with_tstamp_precision(
      DEFAULT_LINK_TYPE, DEFAULT_SNAP_LEN, PCAP_TSTAMP_PRECISION_NANO);
  if (!pcap_) {
    setError("pcap_open_dead_with_tstamp_precision", "failed");
    return false;
  }
  haveNanosec_ = true;
#else   // !HAVE_PCAP_OPEN_DEAD_WITH_TSTAMP_PRECISION
  pcap_ = pcap_open_dead(DEFAULT_LINK_TYPE, DEFAULT_SNAP_LEN);
  if (!pcap_) {
    setError("pcap_open_dead_with_tstamp_precision", "failed");
    return false;
  }
  haveNanosec_ = false;
#endif  // !HAVE_PCAP_OPEN_DEAD_WITH_TSTAMP_PRECISION

  dump_ = pcap_dump_open(pcap_, file.c_str());
  if (!dump_) {
    setError("pcap_dump_open", pcap_geterr(pcap_));
    close();
    return false;
  }

  return true;
}

void PktSink::close() {
  // Do nothing if file is not open.
  if (!pcap_)
    return;

  if (dump_) {
    pcap_dump_close(dump_);
    dump_ = nullptr;
  }

  pcap_close(pcap_);
  pcap_ = nullptr;
}

void PktSink::write(const Timestamp &ts, const ByteRange &captureData,
                    UInt32 length) {
  const UInt32 usec = haveNanosec_ ? ts.nanoseconds() : ts.microseconds();

  struct pcap_pkthdr hdr;
  hdr.ts.tv_sec = ts.seconds();
  hdr.ts.tv_usec = Signed_cast(usec);
  hdr.caplen = UInt32_narrow_cast(captureData.size());
  hdr.len = length;

  pcap_dump(reinterpret_cast<u_char *>(dump_), &hdr, captureData.data());
}

void PktSink::setError(const char *func, const char *msg) {
  std::ostringstream oss;
  oss << func << ": " << msg;
  error_ = oss.str();
}
