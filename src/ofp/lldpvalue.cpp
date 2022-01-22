// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/lldpvalue.h"

#include "ofp/byteorder.h"
#include "ofp/ipv6address.h"
#include "ofp/macaddress.h"
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
                         size_t offset) {
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
  const char *b = reinterpret_cast<const char *>(data.begin()) + offset;
  const char *e = reinterpret_cast<const char *>(data.end());
  auto utf8 = detail::validUtf8String(b, e);
  return tag.empty() ? utf8 : tag + " " + utf8;
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

  return toRaw("unknown", data, 0);
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

  return toRaw("unknown", data, 0);
}

static std::string toAddress(const ByteRange &data, size_t offset) {
  if (data.empty() || offset >= data.size())
    return toRaw("unknown", data, 0);

  switch (data[offset]) {
    case asByte(IANA::IPv4):
      return toAddressV4(data, 2);
    case asByte(IANA::IPv6):
      return toAddressV6(data, 2);
  }

  return toRaw("unknown", data, 0);
}

static bool fromAddressMAC(const std::string &val, ByteList *data,
                           UInt8 subtype) {
  MacAddress addr;
  if (!addr.parse(val))
    return false;
  data->clear();
  data->add(&subtype, 1);
  data->add(&addr, sizeof(addr));
  return true;
}

static std::string toAddressMAC(const ByteRange &data, size_t offset) {
  MacAddress addr;
  if (offset + sizeof(addr) == data.size()) {
    std::memcpy(&addr, &data[offset], sizeof(addr));
    return "mac " + addr.toString();
  }

  return toRaw("unknown", data, 0);
}

// Convert LLDP ChassisID to a string.
//
// Formatted as:
//     LocallyAssigned      <text>
//     ChassisComponent     chassis <hex>
//     InterfaceAlias       ifalias <text>
//     PortComponent        port <hex>
//     MacAddress           mac <hex>
//     NetworkAddress       ip <ipv4>
//     NetworkAddress       ip6 <ipv6>
//     InterfaceName        ifname <text>
//     Other subtype        unknown <hex>   (Hex includes subtype byte)

static std::string chassisIDToString(const ByteRange &data) {
  if (data.empty()) {
    return "unknown";
  }

  switch (data[0]) {
    case asByte(ChassisIDSubtype::ChassisComponent):
      return toRaw("chassis", data, 1);
    case asByte(ChassisIDSubtype::InterfaceAlias):
      return toText("ifalias", data, 1);
    case asByte(ChassisIDSubtype::PortComponent):
      return toRaw("port", data, 1);
    case asByte(ChassisIDSubtype::MacAddress):
      return toAddressMAC(data, 1);
    case asByte(ChassisIDSubtype::NetworkAddress):
      return toAddress(data, 1);
    case asByte(ChassisIDSubtype::InterfaceName):
      return toText("ifname", data, 1);
    case asByte(ChassisIDSubtype::LocallyAssigned):
      return toText("", data, 1);
  }

  return toRaw("unknown", data, 0);
}

// Convert string to LLDP ChassisID.
//
// Formats accepted:
//     <text>                   LocallyAssigned
//     ifalias <text>           InterfaceAlias
//     chassis <hex>            ChassisComponent
//     port <hex>               PortComponent
//     ifname <text>            InterfaceName
//     mac <hex>                MacAddress
//     ip <ipv4>                NetworkAddress
//     ip6 <ipv6>               NetworkAddress
//     unknown <hex>            Hex includes subtype byte

static bool chassisIDFromString(const std::string &val, ByteList *data) {
  auto pair = llvm::StringRef{val}.split(' ');

  if (pair.first == "unknown")
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
    return fromAddressMAC(pair.second, data,
                          asByte(ChassisIDSubtype::MacAddress));

  if (pair.first == "ip")
    return fromAddressV4(pair.second, data,
                         asByte(ChassisIDSubtype::NetworkAddress));

  if (pair.first == "ip6")
    return fromAddressV6(pair.second, data,
                         asByte(ChassisIDSubtype::NetworkAddress));

  return fromText(val, data, asByte(ChassisIDSubtype::LocallyAssigned));
}

static std::string portIDToString(const ByteRange &data) {
  if (data.empty()) {
    return "unknown";
  }

  switch (data[0]) {
    case asByte(PortIDSubtype::InterfaceAlias):
      return toRaw("ifalias", data, 1);
    case asByte(PortIDSubtype::PortComponent):
      return toRaw("port", data, 1);
    case asByte(PortIDSubtype::NetworkAddress):
      return toAddress(data, 1);
    case asByte(PortIDSubtype::MacAddress):
      return toAddressMAC(data, 1);
    case asByte(PortIDSubtype::InterfaceName):
      return toText("ifname", data, 1);
    case asByte(PortIDSubtype::AgentCircuitID):
      return toRaw("circuit", data, 1);
    case asByte(PortIDSubtype::LocallyAssigned):
      return toText("", data, 1);
  }

  return toRaw("unknown", data, 0);
}

static bool portIDFromString(const std::string &val, ByteList *data) {
  auto pair = llvm::StringRef{val}.split(' ');

  if (pair.first == "unknown")
    return fromRaw(pair.second, data, 0);

  if (pair.first == "ifalias")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::InterfaceAlias));

  if (pair.first == "port")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::PortComponent));

  if (pair.first == "ifname")
    return fromText(pair.second, data, asByte(PortIDSubtype::InterfaceName));

  if (pair.first == "mac")
    return fromAddressMAC(pair.second, data, asByte(PortIDSubtype::MacAddress));

  if (pair.first == "ip")
    return fromAddressV4(pair.second, data,
                         asByte(PortIDSubtype::NetworkAddress));

  if (pair.first == "ip6")
    return fromAddressV6(pair.second, data,
                         asByte(PortIDSubtype::NetworkAddress));

  if (pair.first == "circuit")
    return fromRaw(pair.second, data, asByte(PortIDSubtype::AgentCircuitID));

  return fromText(val, data, asByte(PortIDSubtype::LocallyAssigned));
}

static std::string orgSpecificToString(const ByteRange &data) {
  if (data.size() < 4) {
    return toRaw("unknown", data, 0);
  }

  UInt32 org_prefix = Big32_unaligned(data.data());
  UInt32 oui = org_prefix >> 8;
  UInt8 subtype = org_prefix & 0xFF;
  ByteRange rest = SafeByteRange(data, 4);

  std::string result;
  llvm::raw_string_ostream oss{result};
  oss << "0x";
  oss.write_hex(oui);
  oss << " 0x";
  oss.write_hex(subtype);
  oss << ' ' << RawDataToHex(rest.data(), rest.size());

  return oss.str();
}

static bool orgSpecificFromString(const std::string &val, ByteList *data) {
  llvm::StringRef str{val};

  UInt32 oui = 0;
  if (str.consumeInteger(0, oui)) {
    log_debug("orgSpecificFromString: Unrecognized oui:", str);
    return false;
  }
  str = str.ltrim();

  UInt8 subtype = 0;
  if (str.consumeInteger(0, subtype)) {
    log_debug("orgSpecificFromString: Unrecognized subtype:", str);
    return false;
  }
  str = str.ltrim();

  data->resize(str.size() / 2 + 1);
  bool err = false;
  size_t size = HexToRawData(str, data->mutableData(), data->size(), &err);
  data->resize(size);
  if (err) {
    return false;
  }

  Big32 prefix = (oui << 8) | subtype;
  data->insert(data->begin(), &prefix, sizeof(prefix));
  return true;
}

/// Parse a string into the value of a LLDP TLV. The interpretation of the
/// string depends on the type of TLV.
bool ofp::detail::LLDPParse(LLDPType type, const std::string &val,
                            ByteList *data) {
  switch (type) {
    case LLDPType::ChassisID:
      return chassisIDFromString(val, data);
    case LLDPType::PortID:
      return portIDFromString(val, data);
    case LLDPType::ByteString:
      return fromText(val, data, 0);
    case LLDPType::OrgSpecific:
      return orgSpecificFromString(val, data);
  }

  return fromRaw(val, data, 0);
}

/// Convert a LLDP TLV's byte value into a string.
std::string ofp::detail::LLDPToString(LLDPType type, const ByteRange &data) {
  switch (type) {
    case LLDPType::ChassisID:
      return chassisIDToString(data);
    case LLDPType::PortID:
      return portIDToString(data);
    case LLDPType::ByteString:
      return toText(data);
    case LLDPType::OrgSpecific:
      return orgSpecificToString(data);
  }

  return RawDataToHex(data.begin(), data.size());
}
