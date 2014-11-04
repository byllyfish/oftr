// Copyright 2014-present Bill Fisher. All rights reserved.

#include <sstream>
#include "ofp/ipv6endpoint.h"

using namespace ofp;

static bool scan(std::istream &stream, char delimiter) {
  auto pos = stream.tellg();

  char ch;
  if (stream >> ch && ch == delimiter) {
    return true;
  }

  stream.clear();
  stream.seekg(pos);

  return false;
}

static bool scan(std::istream &stream, std::string &str, char delimiter) {
  auto pos = stream.tellg();

  if (std::getline(stream, str, delimiter) && !stream.eof()) return true;

  stream.clear();
  stream.seekg(pos);

  return false;
}

bool IPv6Endpoint::parse(const std::string &s) {
  std::stringstream iss{s};
  std::string addrStr;

  if (scan(iss, '[')) {
    if (!scan(iss, addrStr, ']')) {
      return false;
    }
    if (!scan(iss, ':')) {
      return false;
    }

  } else if (!scan(iss, addrStr, ':')) {
    addrStr.clear();
  }

  UInt16 port = 0;
  iss >> port;

  if (iss.eof() && port != 0) {
    if (addrStr.empty()) {
      addr_.clear();
    } else if (!addr_.parse(addrStr)) {
      return false;
    }

    port_ = port;
    return true;
  }

  return false;
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
