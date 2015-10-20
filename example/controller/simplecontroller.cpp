// Copyright 2014-present Bill Fisher. All rights reserved.

#include "./simplecontroller.h"

namespace controller {

struct EnetFrame {
  MacAddress dst;
  MacAddress src;
  Big16 type;
};

void SimpleController::onPacketIn(Channel *channel, const PacketIn *msg) {
  const EnetFrame *frame =
      reinterpret_cast<const EnetFrame *>(msg->enetFrame().data());
  if (msg->enetFrame().size() < 14)
    return;

  UInt32 inPort = msg->inPort();
  UInt32 outPort;

  // Update forwarding table.
  if (!frame->src.isMulticast()) {
    fwdTable_[frame->src] = inPort;
  }

  if (frame->dst.isMulticast()) {
    flood(channel, msg);

  } else if (!lookupPort(frame->dst, &outPort)) {
    flood(channel, msg);

  } else if (outPort == inPort) {
    drop(channel, msg, frame, 10);

  } else {
    addFlow(channel, msg, frame, outPort);
  }
}

void SimpleController::onPortStatus(Channel *channel, const PortStatus *msg) {
  log::debug("SimpleController - PortStatus from:", channel);
}

void SimpleController::onError(Channel *channel, const Error *msg) {
  log::debug("SimpleController:: - error");
}

bool SimpleController::lookupPort(const MacAddress &addr, UInt32 *port) const {
  auto iter = fwdTable_.find(addr);
  if (iter != fwdTable_.end()) {
    *port = iter->second;
    return true;
  }
  return false;
}

void SimpleController::flood(Channel *channel, const PacketIn *msg) {
  log::debug("flood");

  ActionList actions;
  actions.add(AT_OUTPUT{OFPP_FLOOD});

  PacketOutBuilder packetOut;
  packetOut.setBufferId(msg->bufferId());
  packetOut.setInPort(msg->inPort());
  packetOut.setActions(actions);
  packetOut.send(channel);
}

void SimpleController::drop(Channel *channel, const PacketIn *msg,
                            const EnetFrame *frame, UInt16 timeout) {
  log::debug("drop");

  if (timeout > 0) {
    MatchBuilder match;
    match.add(OFB_ETH_DST{frame->dst});
    match.add(OFB_ETH_SRC{frame->src});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setBufferId(msg->bufferId());
    flowMod.setIdleTimeout(timeout);
    flowMod.setHardTimeout(timeout);
    flowMod.send(channel);

  } else {
    PacketOutBuilder packetOut;
    packetOut.setBufferId(msg->bufferId());
    packetOut.setInPort(msg->inPort());
    packetOut.send(channel);
  }
}

void SimpleController::addFlow(Channel *channel, const PacketIn *msg,
                               const EnetFrame *frame, UInt32 outPort) {
  MatchBuilder match;
  match.add(OFB_IN_PORT{msg->inPort()});
  match.add(OFB_ETH_DST{frame->dst});
  match.add(OFB_ETH_SRC{frame->src});

  ActionList actions;
  actions.add(AT_OUTPUT{outPort});

  InstructionList instructions;
  instructions.add(IT_APPLY_ACTIONS{&actions});

  FlowModBuilder flowMod;
  flowMod.setMatch(match);
  flowMod.setInstructions(instructions);
  flowMod.setIdleTimeout(10);
  flowMod.setHardTimeout(30);
  flowMod.setPriority(0x7FFF);
  flowMod.setBufferId(msg->bufferId());
  flowMod.send(channel);
}

}  // namespace controller
