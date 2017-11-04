#ifndef OFP_DEMUX_PKTFILTER_H_
#define OFP_DEMUX_PKTFILTER_H_

#include <pcap/pcap.h>
#include "ofp/byterange.h"

namespace ofp {
namespace demux {

OFP_BEGIN_IGNORE_PADDING

/// \brief A concrete class that adapts the bpf_filter API for matching packets.

class PktFilter {
 public:
  PktFilter();
  ~PktFilter();

  bool setFilter(const std::string &filter);

  bool match(ByteRange data) const { return match(data, data.size()); }
  bool match(ByteRange data, size_t totalLen) const;

 private:
  struct bpf_program prog_;
  bool progExists_ = false;
};

OFP_END_IGNORE_PADDING

}  // namespace demux
}  // namespace ofp

#endif  // OFP_DEMUX_PKTFILTER_H_
