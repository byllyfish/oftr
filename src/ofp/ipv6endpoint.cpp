// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/ipv6endpoint.h"
#include "ofp/log.h"

using namespace ofp;

IPv6Endpoint::IPv6Endpoint(const std::string &s) {
  if (!parse(s))
    clear();
}

bool IPv6Endpoint::parse(const std::string &s) {
  using llvm::StringRef;

  StringRef input = StringRef{s}.trim();
  StringRef addrStr;
  StringRef portStr;

  if (input.empty())
    return false;

  if (input[0] == '[') {
    // Check for address enclosed in brackets.
    size_t endBracket = input.find(']');
    if (endBracket == StringRef::npos)
      return false;

    // Obtain address substring and trim leading/trailing spaces.
    addrStr = input.substr(1, endBracket - 1).trim();

    // Grab rest of the input after the ']'
    StringRef restStr = input.substr(endBracket + 1).ltrim();
    if (restStr.empty())
      return false;

    // The rest must begin with a ':' or a '.'
    if (restStr[0] != ':' && restStr[0] != '.')
      return false;

    portStr = restStr.substr(1).ltrim();

  } else {
    // Check for address without brackets.

    size_t lastSeparator = input.rfind(':');
    if (lastSeparator == StringRef::npos) {
      lastSeparator = input.rfind('.');

    } else {
      // There's a ':' in the input. We still need to check for the period
      // separator -- ex: "::1.80" is valid input
      size_t period = input.rfind('.');
      if (period != StringRef::npos && period > lastSeparator)
        lastSeparator = period;
    }

    if (lastSeparator == StringRef::npos) {
      // Input contains the port number only.
      addrStr = "";
      portStr = input;

    } else {
      assert(input[lastSeparator] == ':' || input[lastSeparator] == '.');
      addrStr = input.substr(0, lastSeparator).rtrim();
      portStr = input.substr(lastSeparator + 1).ltrim();
    }
  }

  if (portStr.empty())
    return false;

  // Parse value of base 10 port number. N.B. TRUE result signfies error.
  UInt16 port = 0;
  if (portStr.getAsInteger(10, port) || (port == 0))
    return false;

  if (addrStr.empty()) {
    addr_.clear();
  } else if (!addr_.parse(addrStr)) {
    return false;
  }

  port_ = port;

  return true;
}

void IPv6Endpoint::clear() {
  addr_.clear();
  port_ = 0;
}

std::string IPv6Endpoint::toString() const {
  std::string buf;
  llvm::raw_string_ostream oss{buf};
  oss << *this;
  return oss.str();
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const IPv6Endpoint &value) {
  if (value.addr_.isV4Mapped()) {
    os << value.addr_ << ':';
  } else {
    os << '[' << value.addr_ << "]:" ;
  }
  return os << value.port_;
}

}  // namespace ofp
