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
  int alignment;
  ByteList buffer;
};

OFP_END_IGNORE_PADDING

static void sHandler(u_char *user, const struct pcap_pkthdr *hdr,
                     const u_char *data) {
  sHandlerInfo *info = reinterpret_cast<sHandlerInfo *>(user);
  ByteList &buf = info->buffer;
  Timestamp ts(hdr->ts.tv_sec,
               Unsigned_cast(hdr->ts.tv_usec) * info->nanosec_factor);

  const int align = info->alignment;
  buf.replace(buf.data() + align, buf.end(), data, hdr->caplen);
  ByteRange pkt{buf.data() + align, buf.size() - align};
  info->callback(ts, pkt, hdr->len, info->context);
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

  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_ = pcap_open_offline_with_tstamp_precision(
      path.c_str(), PCAP_TSTAMP_PRECISION_NANO, errbuf);
  if (!pcap_) {
    // Set the error string directly; the pcap error message includes the
    // file name.
    error_ = errbuf;
    return false;
  }

  // pcap library will convert to nanoseconds for us.
  nanosec_factor_ = 1;

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
  }
}

bool PktSource::runLoop(PktCallback callback, void *context) {
  assert(pcap_);

  int kAllPackets = -1;

  sHandlerInfo info;
  info.callback = callback;
  info.context = context;
  info.nanosec_factor = nanosec_factor_;
  info.alignment = encapsulation() == ENCAP_ETHERNET ? 2 : 0;
  info.buffer.addZeros(info.alignment);

  log::debug("PktSource::runLoop");

  int result = pcap_loop(pcap_, kAllPackets, sHandler, MutableBytePtr(&info));
  if (result != 0) {
    log::debug("pcap_loop returned", result);
  }

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

  setTimestampPrecision();

  return true;
}

/// \brief Inspect data link type to determine encapsulation.
/// 
/// \return false if the data link type is not supported
bool PktSource::checkDatalink() {
  assert(pcap_);

  int result = pcap_datalink(pcap_);
  if (result < 0) {
    setError("pcap_datalink", "", "");
    return false;
  }

  encap_ = lookupEncapsulation(result);

  const char *name = pcap_datalink_val_to_name(result);
  if (!name) {
    name = "unknown";
  }
  log::debug("pcap_datalink returned", result, name, encap_);

  return true;
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

void PktSource::setTimestampPrecision() {
  assert(pcap_);

  int *tsTypes = nullptr;
  int tsCount = pcap_list_tstamp_types(pcap_, &tsTypes);
  log::debug("pcap_list_tstamp_types returned", tsCount);
  if (tsCount > 0) {
    for (int i = 0; i < tsCount; ++i) {
      log::debug("tstamp type:", tsTypes[i]);
    }
    pcap_free_tstamp_types(tsTypes);
  } else if (tsCount == PCAP_ERROR) {
    log::debug("pcap_list_tstamp_types:", pcap_geterr(pcap_));
  }

  nanosec_factor_ = 1000;

  // int result = pcap_set_tstamp_type(pcap_, int tstamp_type);
  // if (result) {
  //    setError("pcap_set_tstamp_type", "", "failed");
  //    return false;
  //}
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

  bool operator==(int type) const { return dlType == type; }
};

static const DatalinkInfo sDatalinkInfo[] = {
  { DLT_NULL, PktSource::ENCAP_IP },
  { DLT_EN10MB, PktSource::ENCAP_ETHERNET },
  { DLT_RAW, PktSource::ENCAP_IP },
  //{ DLT_LINUX_SLL, PktSource::ENCAP_ETHERNET },
  //{ DLT_LOOP  , PktSource::ENCAP_IP }
};

PktSource::Encapsulation PktSource::lookupEncapsulation(int datalink) {
  auto iter = std::find(std::begin(sDatalinkInfo), std::end(sDatalinkInfo), datalink);
  if (iter != std::end(sDatalinkInfo)) {
    return iter->encap;
  }
  return ENCAP_UNSUPPORTED;
}
