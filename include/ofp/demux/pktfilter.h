// Copyright (c) 2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_DEMUX_PKTFILTER_H_
#define OFP_DEMUX_PKTFILTER_H_

#include <pcap/pcap.h>
#include "ofp/byterange.h"

namespace ofp {
namespace demux {

OFP_BEGIN_IGNORE_PADDING

/// \brief A concrete class that adapts the bpf_filter API for matching packets.
///
/// This class is movable but not copyable.

class PktFilter {
 public:
  PktFilter() = default;
  PktFilter(PktFilter &&rhs);
  ~PktFilter() { clear(); }

  bool empty() const { return prog_.bf_insns == nullptr; }
  void clear();

  std::string filter() const { return filter_; }
  bool setFilter(const std::string &filter);

  bool match(ByteRange data) const { return match(data, data.size()); }
  bool match(ByteRange data, size_t totalLen) const;

 private:
  struct bpf_program prog_ = {0, nullptr};
  std::string filter_;

  PktFilter(const PktFilter &) = delete;
  PktFilter &operator=(const PktFilter &) = delete;
};

OFP_END_IGNORE_PADDING

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_PKTFILTER_H_
