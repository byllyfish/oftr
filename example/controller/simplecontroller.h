// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef EXAMPLE_CONTROLLER_SIMPLECONTROLLER_H_
#define EXAMPLE_CONTROLLER_SIMPLECONTROLLER_H_

#include <unordered_map>
#include "ofp/ofp.h"

namespace controller {

using namespace ofp;

struct EnetFrame;

class SimpleController {
 public:
  void onPacketIn(Channel *channel, const PacketIn *msg);
  void onPortStatus(Channel *channel, const PortStatus *msg);
  void onError(Channel *channel, const Error *msg);

 private:
  using FwdTable = std::unordered_map<MacAddress, UInt32>;

  FwdTable fwdTable_;

  bool lookupPort(const MacAddress &addr, UInt32 *port) const;

  static void flood(Channel *channel, const PacketIn *msg);
  static void drop(Channel *channel, const PacketIn *msg,
                   const EnetFrame *frame, UInt16 timeout);
  static void addFlow(Channel *channel, const PacketIn *msg,
                      const EnetFrame *frame, UInt32 outPort);
};

}  // namespace controller

#endif  // EXAMPLE_CONTROLLER_SIMPLECONTROLLER_H_
