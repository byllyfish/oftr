// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFPX_PING_H_
#define OFPX_PING_H_

#include "ofpx.h"

namespace ofpx {

class Ping : public Subprogram {
 public:
  int run(int argc, const char *const *argv) override;

 private:
  cl::opt<int> size_{"size", cl::desc("data size"), cl::value_desc("size")};

  cl::opt<ofp::IPv6Endpoint, false, IPv6EndpointParser> endpoint_{
      cl::Positional, cl::desc("<Endpoint>"), cl::ValueRequired};

  // Maximum size of ping message payload is maximum length of OpenFlow message
  // minus header size.
  static const int kMaxPingData = ofp::OFP_MAX_SIZE - 8;

  int ping();
};

}  // namespace ofpx

#endif  // OFPX_PING_H_
