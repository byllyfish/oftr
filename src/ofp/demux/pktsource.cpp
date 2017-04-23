// Copyright (c) 2016-2017 William W. Fisher (at gmail dot com)
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
  UInt32 packetCount;
  ByteList buffer;
};

OFP_END_IGNORE_PADDING

static void sHandler(u_char *user, const struct pcap_pkthdr *hdr,
                     const u_char *data) {
  sHandlerInfo *info = reinterpret_cast<sHandlerInfo *>(user);
  ByteList &buf = info->buffer;
  Timestamp ts(Unsigned_cast(hdr->ts.tv_sec),
               Unsigned_cast(hdr->ts.tv_usec) * info->nanosec_factor);

  const UInt32 alignPad = info->alignPad;
  const UInt32 frameSkip = info->frameSkip;

  if (frameSkip > hdr->caplen) {
    log_debug("PktSource: caplen less than frameSkip!");
    return;
  }

  const u_char *cap = data + frameSkip;
  const size_t caplen = hdr->caplen - frameSkip;

  buf.replace(buf.data() + alignPad, buf.end(), cap, caplen);
  ByteRange pkt{buf.data() + alignPad, buf.size() - alignPad};
  info->callback(ts, pkt, hdr->len, info->context);
  ++info->packetCount;
}

std::string PktSource::datalink() const {
  if (datalink_ < 0) {
    return "<not open>";
  }

  const char *name = pcap_datalink_val_to_name(datalink_);
  if (name == nullptr) {
    return "<null/error>";
  }

  return name;
}

/// \brief Open capture device to read live packets from the network.
///
/// \param device name of capture interface/device
/// \param filter bpf-filter expression
///
/// \returns true if device is ready to use
bool PktSource::openDevice(const std::string &device,
                           const std::string &filter) {
  if (!isPcapVersionSupported())
    return false;

  if (pcap_)
    close();

  if (!create(device)) {
    close();
    return false;
  }

  if (!activate(device)) {
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
bool PktSource::openFile(const std::string &file, const std::string &filter) {
  if (!isPcapVersionSupported())
    return false;

  if (pcap_)
    close();

  if (!openOffline(file)) {
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

  log_debug("PktSource::openFile", file, "filter", filter);

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

  log_debug("PktSource::runLoop entered");

  int kAllPackets = -1;

  sHandlerInfo info;
  info.callback = callback;
  info.context = context;
  info.nanosec_factor = nanosec_factor_;
  info.alignPad = (encapsulation() == ENCAP_ETHERNET) ? 2 : 0;
  info.frameSkip = frameSkip_;
  info.packetCount = packetCount_;
  info.buffer.addZeros(info.alignPad);

  int result = pcap_loop(pcap_, kAllPackets, sHandler, MutableBytePtr(&info));
  if (result != 0) {
    log_debug("pcap_loop returned", result);
  }

  packetCount_ = info.packetCount;

  log_debug("PktSource::runLoop exited");

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
#else   // !HAVE_PCAP_OPEN_OFFLINE_WITH_TSTAMP_PRECISION
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

  if (filter.empty()) {
    // If filter is empty, capture everything.
    fullFilter = "";
  } else if (supportsVlan) {
    // If the datalink supports 802.1q vlans, combine our filter with the
    // magic 'vlan' option.
    std::ostringstream oss;
    oss << "(" << filter << ") or (vlan and (" << filter << "))";
    fullFilter = oss.str();
  } else {
    // Use the filter "as is".
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

bool PktSource::activate(const std::string &device) {
  int result = pcap_activate(pcap_);
  if (result == 0) {
    return true;
  }

  const char *errMsg = "Unknown error";

  switch (result) {
    // Warnings log a message and allow activate to continue.
    case PCAP_WARNING_PROMISC_NOTSUP:
      log_warning("pcap_activate:", device,
                  "does not support promiscuous mode --", pcap_geterr(pcap_));
      return true;
#if defined(PCAP_WARNING_TSTAMP_TYPE_NOTSUP)
    case PCAP_WARNING_TSTAMP_TYPE_NOTSUP:
      log_warning(
          "pcap_activate: Capture source does not support timestamp type");
      return true;
#endif  // defined(PCAP_WARNING_TSTAMP_TYPE_NOTSUP)
    case PCAP_WARNING:
      log_warning("pcap_activate: PCAP_WARNING:", pcap_geterr(pcap_));
      return true;

    // Errors set the error message and return false. Log pcap_geterr to the
    // log.
    case PCAP_ERROR_NO_SUCH_DEVICE:
      errMsg = "No such device";
      break;
    case PCAP_ERROR_ACTIVATED:
      errMsg = "Handle already activated";
      break;
    case PCAP_ERROR_PERM_DENIED:
      errMsg = "Permission denied to open capture source";
      break;
#if defined(PCAP_ERROR_PROMISC_PERM_DENIED)
    case PCAP_ERROR_PROMISC_PERM_DENIED:
      errMsg = "Permission denied to put capture source into promiscuous mode";
      break;
#endif  // defined(PCAP_ERROR_PROMISC_PERM_DENIED)
    case PCAP_ERROR_RFMON_NOTSUP:
      errMsg = "Capture source does not support monitor mode";
      break;
    case PCAP_ERROR_IFACE_NOT_UP:
      errMsg = "Capture source is not up";
      break;
    case PCAP_ERROR:
    default:
      errMsg = pcap_geterr(pcap_);
      break;
  }

  setError("pcap_activate", device, errMsg);
  log_error("pcap_activate:", errMsg, "--", pcap_geterr(pcap_));

  return false;
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
    {DLT_NULL, PktSource::ENCAP_IP, 4},
    {DLT_EN10MB, PktSource::ENCAP_ETHERNET, 0},
    {DLT_RAW, PktSource::ENCAP_IP, 0},
    // TODO(bfish):
    //{ DLT_LINUX_SLL, PktSource::ENCAP_ETHERNET },
    //{ DLT_LOOP  , PktSource::ENCAP_IP }
};

PktSource::Encapsulation PktSource::lookupEncapsulation(int datalink,
                                                        UInt32 *frameSkip) {
  auto iter =
      std::find(std::begin(sDatalinkInfo), std::end(sDatalinkInfo), datalink);
  if (iter != std::end(sDatalinkInfo)) {
    *frameSkip = iter->frameSkip;
    return iter->encap;
  }
  return ENCAP_UNSUPPORTED;
}

/// Return true if we support this version of libpcap (1.1 or newer). Set error_
/// if not. We use version 1.1 for support for Ubuntu Precise.
bool PktSource::isPcapVersionSupported() {
  const char *vers = pcap_lib_version();
  log::fatal_if_null(vers, "pcap_lib_version");

  const unsigned int LIB_MAJOR = 1;
  const unsigned int LIB_MINOR = 1;

  unsigned int major = 0;
  unsigned int minor = 0;
  unsigned int patch = 0;

  if (std::sscanf(vers, "libpcap version %u.%u.%u", &major, &minor, &patch) <
      2) {
    setError("pcap_lib_version", "", vers);
    return false;
  }

  if (major > LIB_MAJOR || (major == LIB_MAJOR && minor >= LIB_MINOR)) {
    return true;
  }

  error_ = "Unsupported: ";
  error_ += vers;

  return false;
}
