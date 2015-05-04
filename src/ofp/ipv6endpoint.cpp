// Copyright 2014-present Bill Fisher. All rights reserved.

#include "llvm/ADT/StringRef.h"
#include "ofp/ipv6endpoint.h"
#include "ofp/log.h"

using namespace ofp;

bool IPv6Endpoint::parse(const std::string &s) {
  using llvm::StringRef;

  StringRef input = StringRef{s}.trim();
  StringRef addrStr;
  StringRef portStr;

  if (input.empty())
    return false;

  log::debug("IPv6Endpoint::parseInput ", input);

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
      if (lastSeparator == StringRef::npos)
        return false;

    } else {
      // There's a ':' in the input. We still need to check for the period
      // separator -- ex: "::1.80" is valid input
      size_t period = input.rfind('.');
      if (period != StringRef::npos && period > lastSeparator)
        lastSeparator = period;
    }

    assert(input[lastSeparator] == ':' || input[lastSeparator] == '.');

    addrStr = input.substr(0, lastSeparator).rtrim();
    portStr = input.substr(lastSeparator + 1).ltrim();
  }

  log::debug("IPv6Endpoint::parse ", addrStr, portStr);

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
  std::string result;

  if (addr_.isV4Mapped()) {
    result += addr_.toString();
  } else {
    result += '[';
    result += addr_.toString();
    result += ']';
  }

  result += ':';
  result += std::to_string(port_);

  return result;
}
