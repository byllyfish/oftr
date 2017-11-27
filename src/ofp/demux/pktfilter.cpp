// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/pktfilter.h"
#include <pcap/bpf.h>
#include "ofp/log.h"

using namespace ofp::demux;

// In the following member-wise initialization, we need to use 
// `prog_(rhs.prog_)`. Using braces results in a GCC-4.8.4 compiler error.

PktFilter::PktFilter(PktFilter &&rhs) : prog_(rhs.prog_), filter_{std::move(rhs.filter_)} {
  rhs.prog_.bf_insns = nullptr;
  assert(rhs.filter_.empty());
}

void PktFilter::clear() {
  if (!empty()) {
    pcap_freecode(&prog_);
    filter_.clear();
  }
  assert(empty());
}

bool PktFilter::setFilter(const std::string &filter) {
  const int kOptimize = 1;
  const int kSnapLen = 65535;
  const int kLinkType = DLT_EN10MB;

  struct bpf_program newProg = { 0, nullptr };
  int ret = pcap_compile_nopcap(kSnapLen, kLinkType, &newProg, filter.c_str(),
                                kOptimize, PCAP_NETMASK_UNKNOWN);
  if (ret < 0) {
    assert(newProg.bf_insns == nullptr);
    log_error("pcap_compile_nopcap failed for filter:", filter);
    return false;
  }

  clear();
  prog_ = newProg;
  filter_ = filter;

  return true;
}

bool PktFilter::match(ByteRange data, size_t totalLen) const {
  // Empty filter never matches.
  if (empty()) 
    return false;

  struct pcap_pkthdr hdr;
  hdr.ts.tv_sec = 0;
  hdr.ts.tv_usec = 0;
  hdr.caplen = UInt32_narrow_cast(data.size());
  hdr.len = UInt32_narrow_cast(totalLen);

  assert(prog_.bf_insns != nullptr);
  return pcap_offline_filter(&prog_, &hdr, data.data()) > 0;
}
