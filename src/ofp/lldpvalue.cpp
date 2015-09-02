// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/lldpvalue.h"
#include "ofp/ipv6address.h"
#include "ofp/smallcstring.h"  // for validUtf8String

using namespace ofp;

enum class IANA { IPv4 = 1, IPv6 = 2 };

constexpr UInt8 asByte(IANA val) {
  return static_cast<UInt8>(val);
}

enum class ChassisIDSubtype {
  ChassisComponent = 1,
  InterfaceAlias = 2,
  PortComponent = 3,
  MacAddress = 4,
  NetworkAddress = 5,
  InterfaceName = 6,
  LocallyAssigned = 7,
};

constexpr UInt8 asByte(ChassisIDSubtype val) {
  return static_cast<UInt8>(val);
}

enum class PortIDSubtype {
  InterfaceAlias = 1,
  PortComponent = 2,
  MacAddress = 3,
  NetworkAddress = 4,
  InterfaceName = 5,
  AgentCircuitID = 6,
  LocallyAssigned = 7,
};

constexpr UInt8 asByte(PortIDSubtype val) {
  return static_cast<UInt8>(val);
}

static bool fromRaw(const std::string &val, ByteList *data, UInt8 subtype) {
  data->resize(val.size() / 2 + sizeof(subtype));
  bool err = false;
  size_t size = HexToRawData(val, data->mutableData(), data->size(), &err);
  if (err)
    return false;
  data->resize(size);
  if (subtype)
    data->insert(data->begin(), &subtype, sizeof(subtype));
  return true;
}

static std::string toRaw(const std::string &tag, const ByteRange &data,
                         size_t offset = 0) {
  assert(offset <= data.size());
  assert(!tag.empty());
  return tag + " " + RawDataToHex(data.data() + offset, data.size() - offset);
}

static bool fromText(const std::string &val, ByteList *data, UInt8 subtype) {
  data->set(val.c_str(), val.size());
  if (subtype)
    data->insert(data->begin(), &subtype, sizeof(subtype));
  return true;
}

static std::string toText(const ByteRange &data) {
  const char *s = reinterpret_cast<const char *>(data.data());
  return detail::validUtf8String(s, s + data.size());
}

static std::string toText(const std::string &tag, const ByteRange &data,
                          size_t offset = 0) {
  assert(offset <= data.size());
  assert(!tag.empty());
  const char *b = reinterpret_cast<const char *>(data.begin()) + offset;
  const char *e = reinterpret_cast<const char *>(data.end());
  return tag + " " + detail::validUtf8String(b, e);
}

static bool fromAddressV4(const std::string &val, ByteList *data,
                          UInt8 subtype) {
  IPv4Address addr;
  if (!addr.parse(val))
    return false;
  data->resize(2 + sizeof(addr));
  UInt8 *buf = data->mutableData();
  buf[0] = subtype;
  buf[1] = asByte(IANA::IPv4);
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

static bool fromAddressV6(const std::string &val, ByteList *data,
                          UInt8 subtype) {
  IPv6Address addr;
  if (!addr.parse(val))
    return false;
  data->resize(2 + sizeof(addr));
  UInt8 *buf = data->mutableData();
  buf[0] = subtype;
  buf[1] = asByte(IANA::IPv6);
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
    case asByte(IANA::IPv4):
      return toAddressV4(data, 2);
    case asByte(IANA::IPv6):
      return toAddressV6(data, 2);
  }

  return toRaw("raw", data);
}

// Formatted as:
//     chassis <hex>
//     ifalias <text>
//     port <hex>
//     mac <hex>
//     ip <ipv4>
//     ip6 <ipv6>
//     ifname <text>
//     local <hex>
//
static std::string chassisIDToString(const ByteRange &data) {
  assert(!data.empty());

  switch (data[0]) {
    case asByte(ChassisIDSubtype::ChassisComponent):
      return toRaw("chassis", data, 1);
    case asByte(ChassisIDSubtype::InterfaceAlias):
      return toText("ifalias", data, 1);
    case asByte(ChassisIDSubtype::PortComponent):
      return toRaw("port", data, 1);
    case asByte(ChassisIDSubtype::MacAddress):
      return toRaw("mac", data, 1);
    case asByte(ChassisIDSubtype::NetworkAddress):
      return toAddress(data, 1);
    case asByte(ChassisIDSubtype::InterfaceName):
      return toText("ifname", data, 1);
    case asByte(ChassisIDSubtype::LocallyAssigned):
      return toRaw("local", data, 1);
  }

  return toRaw("raw", data);
}

// Formats accepted:
//     <hex>
//     raw <hex>
//     ifalias <text>
//     chassis <hex>
//     port <hex>
//     ifname <text>
//     mac <hex>
//     ip <ipv4>
//     ip6 <ipv6>
//     local <hex>
//
static bool chassisIDFromString(const std::string &val, ByteList *data) {
  auto pair = llvm::StringRef{val}.split(' ');

  if (pair.first == "raw")
    return fromRaw(pair.second, data, 0);

  if (pair.first == "ifalias")
    return fromText(pair.second, data,
                    asByte(ChassisIDSubtype::InterfaceAlias));

  if (pair.first == "chassis")
    return fromRaw(pair.second, data,
                   asByte(ChassisIDSubtype::ChassisComponent));

  if (pair.first == "port")
    return fromRaw(pair.second, data, asByte(ChassisIDSubtype::PortComponent));

  if (pair.first == "ifname")
    return fromText(pair.second, data, asByte(ChassisIDSubtype::InterfaceName));

  if (pair.first == "mac")
    return fromRaw(pair.second, data, asByte(ChassisIDSubtype::MacAddress));

  if (pair.first == "ip")
    return fromAddressV4(pair.second, data,
                         asByte(ChassisIDSubtype::NetworkAddress));

  if (pair.first == "ip6")
    return fromAddressV6(pair.second, data,
                         asByte(ChassisIDSubtype::NetworkAddress));

  if (pair.first == "local")
    return fromRaw(pair.second, data,
                   asByte(ChassisIDSubtype::LocallyAssigned));

  return fromRaw(val, data, 0);
}

static std::string portIDToString(const ByteRange &data) {
  assert(!data.empty());

  switch (data[0]) {
    case asByte(PortIDSubtype::InterfaceAlias):
      return toRaw("ifalias", data, 1);
    case asByte(PortIDSubtype::PortComponent):
      return toRaw("port", data, 1);
    case asByte(PortIDSubtype::NetworkAddress):
      return toAddress(data, 1);
    case asByte(PortIDSubtype::MacAddress):
      return toRaw("mac", data, 1);
    case asByte(PortIDSubtype::InterfaceName):
      return toRaw("ifname", data, 1);
    case asByte(PortIDSubtype::AgentCircuitID):
      return toRaw("circuit", data, 1);
    case asByte(PortIDSubtype::LocallyAssigned):
      return toRaw("local", data, 1);
  }

  return toRaw("raw", data);
}

static bool portIDFromString(const std::string &val, ByteList *data) {
  auto pair = llvm::StringRef{val}.split(' ');

  if (pair.first == "raw")
    return fromRaw(pair.second, data, 0);

  if (pair.first == "ifalias")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::InterfaceAlias));

  if (pair.first == "port")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::PortComponent));

  if (pair.first == "ifname")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::InterfaceName));

  if (pair.first == "mac")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::MacAddress));

  if (pair.first == "ip")
    return fromAddressV4(pair.second, data,
                         asByte(PortIDSubtype::NetworkAddress));

  if (pair.first == "ip6")
    return fromAddressV6(pair.second, data,
                         asByte(PortIDSubtype::NetworkAddress));

  if (pair.first == "circuit")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::AgentCircuitID));

  if (pair.first == "local")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::LocallyAssigned));

  return fromRaw(val, data, 0);
}

/// Parse a string into the value of a LLDP TLV. The interpretation of the
/// string
/// depends on the type of TLV.
bool ofp::detail::LLDPParse(LLDPType type, const std::string &val,
                            ByteList *data) {
  switch (type) {
    case LLDPType::ChassisID:
      return chassisIDFromString(val, data);
    case LLDPType::PortID:
      return portIDFromString(val, data);
    case LLDPType::PortDescr:
    case LLDPType::SysName:
    case LLDPType::SysDescr:
      return fromText(val, data, 0);
    case LLDPType::SysCapabilities:
    case LLDPType::MgmtAddress:
      return fromRaw(val, data, 0);
  }

  return fromRaw(val, data, 0);
}

/// Convert a LLDP TLV's byte value into a string.
std::string ofp::detail::LLDPToString(LLDPType type, const ByteRange &data) {
  if (data.empty())
    return "";

  switch (type) {
    case LLDPType::ChassisID:
      return chassisIDToString(data);
    case LLDPType::PortID:
      return portIDToString(data);
    case LLDPType::PortDescr:
    case LLDPType::SysName:
    case LLDPType::SysDescr:
      return toText(data);
    case LLDPType::SysCapabilities:
    case LLDPType::MgmtAddress:
      break;
  }

  return RawDataToHex(data.begin(), data.size());
}
