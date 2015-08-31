#include "ofp/lldpvalue.h"
#include "ofp/ipv6address.h"

using namespace ofp;

enum {
    kIANA_IPv4 = 1,
    kIANA_IPv6 = 2
};

enum ChassisIDSubtype {
    kChassisComponent = 1,
    kInterfaceAlias = 2,
    kPortComponent = 3,
    kMacAddress = 4,
    kNetworkAddress = 5,
    kInterfaceName = 6,
    kLocallyAssigned = 7
};


static bool fromRaw(const std::string &val, ByteList *data, unsigned subtype) {
    data->resize(val.size() / 2 + 2);
    bool err = false;
    size_t size = HexToRawData(val, data->mutableData(), data->size(), &err);
    if (err)
        return false;
    data->resize(size);
    if (subtype) {
        UInt8 b = UInt8_narrow_cast(subtype);
        data->insert(data->begin(), &b, 1);
    }
    return true;
}

static std::string toRaw(const std::string &tag, const ByteRange &data, size_t offset = 0) {
  assert(offset < data.size());
  return tag + " " + RawDataToHex(data.data() + offset, data.size() - offset);
}


static bool fromAddressV4(const std::string &val, ByteList *data) {
    IPv4Address addr;
    if (!addr.parse(val))
        return false;
    data->resize(2 + sizeof(addr));
    UInt8 *buf = data->mutableData();
    buf[0] = kNetworkAddress;
    buf[1] = kIANA_IPv4;
    std::memcpy(&buf[2], &addr, sizeof(addr));
    return true;
}

static std::string toAddressV4(const ByteRange &data, size_t offset) {
    IPv4Address addr;
    if (offset + sizeof(addr) == data.size()) {
        std::memcpy(&addr, &data[offset], sizeof(addr));
        return "ip " + addr.toString();
    }

    return toRaw("raw", data);
}

static bool fromAddressV6(const std::string &val, ByteList *data) {
    IPv6Address addr;
    if (!addr.parse(val))
        return false;
    data->resize(2 + sizeof(addr));
    UInt8 *buf = data->mutableData();
    buf[0] = kNetworkAddress;
    buf[1] = kIANA_IPv6;
    std::memcpy(&buf[2], &addr, sizeof(addr));
    return true;
}

static std::string toAddressV6(const ByteRange &data, size_t offset) {
    IPv6Address addr;
    if (offset + sizeof(addr) == data.size()) {
        std::memcpy(&addr, &data[offset], sizeof(addr));
        return "ip6 " + addr.toString();
    }

    return toRaw("raw", data);
}

static std::string toAddress(const ByteRange &data, size_t offset) {
    if (data.empty() || offset >= data.size())
        return toRaw("raw", data);

    switch (data[offset]) {
        case kIANA_IPv4:
            return toAddressV4(data, 2);
        case kIANA_IPv6:
            return toAddressV6(data, 2);
    }

    return toRaw("raw", data);
}

static std::string chassisIDToString(const ByteRange &data) {
    if (data.empty())
        return toRaw("raw", data);

    switch (data[0]) {
        case kChassisComponent:
            return toRaw("chassis", data, 1);
        case kInterfaceAlias:
            return toRaw("ifalias", data, 1);
        case kPortComponent:
            return toRaw("port", data, 1);
        case kMacAddress:
            return toRaw("mac", data, 1);
        case kNetworkAddress:
            return toAddress(data, 1);
        case kInterfaceName:
            return toRaw("ifname", data, 1);
    }

    return toRaw("raw", data);
}


static bool chassisIDFromString(const std::string &val, ByteList *data) {
    auto pair = llvm::StringRef{val}.split(' ');

    if (pair.first == "raw")
        return fromRaw(pair.second, data, 0);

    if (pair.first == "ifalias")
        return fromRaw(pair.second, data, kInterfaceAlias);

    if (pair.first == "chassis")
        return fromRaw(pair.second, data, kChassisComponent);

    if (pair.first == "port")
        return fromRaw(pair.second, data, kPortComponent);
    
    if (pair.first == "ifname")
        return fromRaw(pair.second, data, kInterfaceName);

    if (pair.first == "mac")
        return fromRaw(pair.second, data, kMacAddress);
    
    if (pair.first == "ip")
        return fromAddressV4(pair.second, data);
    
    if (pair.first == "ip6")
        return fromAddressV6(pair.second, data);

    return fromRaw(val, data, 0);
}

/// Parse a string into the value of a LLDP TLV. The interpretation of the string
/// depends on the type of TLV.
bool ofp::detail::LLDPParse(LLDPType type, const std::string &val, ByteList *data) {
    switch (type) {
        case LLDPType::ChassisID:
            return chassisIDFromString(val, data);
    }

    return fromRaw(val, data, 0);
}

/// Convert a LLDP TLV's byte value into a string.
std::string ofp::detail::LLDPToString(LLDPType type, const ByteRange &data) {
  switch (type) {
    case LLDPType::ChassisID:
      return chassisIDToString(data);
  }

  return toRaw("raw", data);
}
