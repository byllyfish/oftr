// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/pktfilter.h"
#include <pcap/bpf.h>
#include "ofp/log.h"

using namespace ofp::demux;

PktFilter::PktFilter() {
  std::memset(&prog_, 0, sizeof(prog_));
}

PktFilter::~PktFilter() {
  if (progExists_) {
    pcap_freecode(&prog_);
  }
}

bool PktFilter::setFilter(const std::string &filter) {
  assert(!progExists_);

  const int kOptimize = 1;
  const int kSnapLen = 65535;
  const int kLinkType = DLT_EN10MB;

  int ret = pcap_compile_nopcap(kSnapLen, kLinkType, &prog_, filter.c_str(),
                                kOptimize, PCAP_NETMASK_UNKNOWN);
  if (ret < 0) {
    log_error("pcap_compile_nopcap failed for filter:", filter);
    return false;
  }

  progExists_ = true;
  return true;
}

bool PktFilter::match(ByteRange data, size_t totalLen) const {
  if (!progExists_)
    return true;

  struct pcap_pkthdr hdr;
  hdr.ts.tv_sec = 0;
  hdr.ts.tv_usec = 0;
  hdr.caplen = UInt32_narrow_cast(data.size());
  hdr.len = UInt32_narrow_cast(totalLen);

  return pcap_offline_filter(&prog_, &hdr, data.data()) > 0;
}
