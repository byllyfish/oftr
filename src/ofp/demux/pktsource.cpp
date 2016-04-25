// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/pktsource.h"
#include <sstream>
#include "ofp/bytelist.h"
#include "ofp/log.h"

using namespace ofp;
using namespace ofp::demux;

OFP_BEGIN_IGNORE_PADDING

struct sHandlerInfo {
  PktSource::PktCallback callback;
  void *context;
  UInt32 nanosec_factor;
  UInt32 alignPad;
  UInt32 frameSkip;
  ByteList buffer;
};

OFP_END_IGNORE_PADDING

static void sHandler(u_char *user, const struct pcap_pkthdr *hdr,
                     const u_char *data) {
  sHandlerInfo *info = reinterpret_cast<sHandlerInfo *>(user);
  ByteList &buf = info->buffer;
  Timestamp ts(hdr->ts.tv_sec,
               Unsigned_cast(hdr->ts.tv_usec) * info->nanosec_factor);

  const UInt32 alignPad = info->alignPad;
  const UInt32 frameSkip = info->frameSkip;

  if (frameSkip > hdr->caplen) {
    log::debug("PktSource: caplen less than frameSkip!");
    return;
  }

  const u_char *cap = data + frameSkip;
  const size_t caplen = hdr->caplen - frameSkip;

  buf.replace(buf.data() + alignPad, buf.end(), cap, caplen);
  ByteRange pkt{buf.data() + alignPad, buf.size() - alignPad};
  info->callback(ts, pkt, hdr->len, info->context);
}

std::string PktSource::datalink() const {
  if (datalink_ >= 0) {
    const char *name = pcap_datalink_val_to_name(datalink_);
    return !name ? "NULL" : name;
  } else {
    return "<not open>";
  }
}

/// \brief Open capture device to read live packets from the network.
///
/// \param source name of capture interface/device
/// \param filter bpf-filter expression
///
/// \returns true if source is ready to use
bool PktSource::openDevice(const std::string &source,
                           const std::string &filter) {
  if (pcap_)
    close();

  if (!create(source)) {
    close();
    return false;
  }

  if (!activate()) {
    close();
    return false;
  }

  if (!checkDatalink()) {
    close();
    return false;
  }

  if (!setFilter(filter)) {
    close();
    return false;
  }

  return true;
}

/// \brief Open capture file to read packets offline.
bool PktSource::openFile(const std::string &path, const std::string &filter) {
  if (pcap_)
    close();

  if (!openOffline(path)) {
    return false;
  }

  // Verify we support this datalink type.
  if (!checkDatalink()) {
    close();
    return false;
  }
  
  // Set the appropriate bpf filter.
  if (!setFilter(filter)) {
    close();
    return false;
  }

  log::debug("PktSource::openFile", path, "filter", filter);

  return true;
}

/// \brief Close the pcap_t object, if it's open.
///
/// Do nothing if it's already closed.
void PktSource::close() {
  if (pcap_) {
    pcap_close(pcap_);
    pcap_ = nullptr;
    encap_ = ENCAP_UNSUPPORTED;
    datalink_ = -1;
  }
}

bool PktSource::runLoop(PktCallback callback, void *context) {
  assert(pcap_);

  log::debug("PktSource::runLoop entered");

  int kAllPackets = -1;

  sHandlerInfo info;
  info.callback = callback;
  info.context = context;
  info.nanosec_factor = nanosec_factor_;
  info.alignPad = (encapsulation() == ENCAP_ETHERNET) ? 2 : 0;
  info.frameSkip = frameSkip_;
  info.buffer.addZeros(info.alignPad);

  int result = pcap_loop(pcap_, kAllPackets, sHandler, MutableBytePtr(&info));
  if (result != 0) {
    log::debug("pcap_loop returned", result);
  }

  log::debug("PktSource::runLoop exited");

  return result == 0;
}

bool PktSource::create(const std::string &device) {
  assert(pcap_ == nullptr);

  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_ = pcap_create(device.c_str(), errbuf);
  if (!pcap_) {
    setError("pcap_create", device, errbuf);
    return false;
  }

  int result = pcap_set_snaplen(pcap_, DEFAULT_SNAPLEN);
  if (result) {
    setError("pcap_set_snaplen", device, "failed");
    return false;
  }

  result = pcap_set_promisc(pcap_, DEFAULT_PROMISC);
  if (result) {
    setError("pcap_set_promisc", device, "failed");
    return false;
  }

  result = pcap_set_timeout(pcap_, DEFAULT_TIMEOUT);
  if (result) {
    setError("pcap_set_timeout failed", device, "failed");
    return false;
  }

  result = pcap_set_buffer_size(pcap_, DEFAULT_BUFFER_SIZE);
  if (result) {
    setError("pcap_set_buffer_size", device, "failed");
    return false;
  }

  return true;
}


bool PktSource::openOffline(const std::string &path) {
  char errbuf[PCAP_ERRBUF_SIZE];

#if HAVE_PCAP_OPEN_OFFLINE_WITH_TSTAMP_PRECISION
  // Use the newer `pcap_open_offline_with_tstamp_precision` API if it is
  // available, since it provides access to nanosecond precision timestamps.
  pcap_ = pcap_open_offline_with_tstamp_precision(
      path.c_str(), PCAP_TSTAMP_PRECISION_NANO, errbuf);
  // pcap library will convert to nanoseconds for us.
  nanosec_factor_ = 1;
#else // !HAVE_PCAP_OPEN_OFFLINE_WITH_TSTAMP_PRECISION
  // Use the original `pcap_open_offline` API if the newer API is not available.
  pcap_ = pcap_open_offline(path.c_str(), errbuf);
  // Convert microseconds to nanoseconds.
  nanosec_factor_ = 1000;
#endif  // !HAVE_PCAP_OPEN_OFFLINE_WITH_TSTAMP_PRECISION

  if (!pcap_) {
    // Set the error string directly; the pcap error message includes the
    // file name.
    error_ = errbuf;
    return false;
  }

  return true;
}

/// \brief Inspect data link type to determine encapsulation.
/// 
/// \return false if the data link type is not supported
bool PktSource::checkDatalink() {
  assert(pcap_);

  datalink_ = pcap_datalink(pcap_);
  if (datalink_ < 0) {
    setError("pcap_datalink", "", "");
    return false;
  }

  encap_ = lookupEncapsulation(datalink_, &frameSkip_);

  return encap_ != ENCAP_UNSUPPORTED;
}

bool PktSource::setFilter(const std::string &filter) {
  assert(pcap_);

  std::string fullFilter;
  bool supportsVlan = encapsulation() == ENCAP_ETHERNET;
  
  if (supportsVlan) {
    std::ostringstream oss;
    oss << "(" << filter << ") or (vlan and (" << filter << "))";
    fullFilter = oss.str();
  } else {
    fullFilter = filter;
  }

  struct bpf_program prog;
  int result =
      pcap_compile(pcap_, &prog, fullFilter.c_str(), 1, PCAP_NETMASK_UNKNOWN);
  if (result < 0) {
    setError("pcap_compile", fullFilter, pcap_geterr(pcap_));
    return false;
  }

  result = pcap_setfilter(pcap_, &prog);
  if (result < 0) {
    setError("pcap_setfilter", fullFilter, pcap_geterr(pcap_));
  }

  pcap_freecode(&prog);

  return (result == 0);
}

bool PktSource::activate() {
  int result = pcap_activate(pcap_);
  if (result == 0) {
    return true;
  }

  // FIXME(bfish): Fill out the rest of this code.
  switch (result) {
    case PCAP_WARNING_PROMISC_NOTSUP:
    case PCAP_WARNING_TSTAMP_TYPE_NOTSUP:
    case PCAP_WARNING:
    case PCAP_ERROR_ACTIVATED:
    case PCAP_ERROR_NO_SUCH_DEVICE:
    case PCAP_ERROR_PERM_DENIED:
    case PCAP_ERROR_PROMISC_PERM_DENIED:
    case PCAP_ERROR_RFMON_NOTSUP:
    case PCAP_ERROR_IFACE_NOT_UP:
    default:
      setError("pcap_activate", "", "");
      break;
  }

  return result > 0;
}

void PktSource::setError(const char *func, const std::string &arg,
                         const char *result) {
  std::ostringstream oss;
  oss << func << "(" << arg << "): " << result;
  error_ = oss.str();
}


struct DatalinkInfo {
  int dlType;
  PktSource::Encapsulation encap;
  UInt32 frameSkip;

  bool operator==(int type) const { return dlType == type; }
};

static const DatalinkInfo sDatalinkInfo[] = {
  { DLT_NULL, PktSource::ENCAP_IP, 4 },
  { DLT_EN10MB, PktSource::ENCAP_ETHERNET, 0 },
  { DLT_RAW, PktSource::ENCAP_IP, 0 },
  // TODO(bfish):
  //{ DLT_LINUX_SLL, PktSource::ENCAP_ETHERNET },
  //{ DLT_LOOP  , PktSource::ENCAP_IP }
};

PktSource::Encapsulation PktSource::lookupEncapsulation(int datalink, UInt32 *frameSkip) {
  auto iter = std::find(std::begin(sDatalinkInfo), std::end(sDatalinkInfo), datalink);
  if (iter != std::end(sDatalinkInfo)) {
    *frameSkip = iter->frameSkip;
    return iter->encap;
  }
  return ENCAP_UNSUPPORTED;
}
