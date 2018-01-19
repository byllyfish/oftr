// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/ofp.h"

using namespace ofp;

class CBenchReplyController : public ChannelListener {
 public:
  void onChannelUp(Channel *channel) override {
    log_info("ChannelUp", channel->remoteEndpoint());
  }

  void onMessage(Message *message) override {
    if (message->type() != PacketIn::type()) {
      // Ignore everything except PacketIn msg.
      return;
    }

    auto msg = PacketIn::cast(message);
    if (!msg) {
      log_warning("Unable to decode PacketIn msg");
      return;
    }

    struct EnetFrame {
      MacAddress dst;
      MacAddress src;
      Big16 type;
    };

    ByteRange data = msg->enetFrame();
    if (data.size() < sizeof(EnetFrame)) {
      log_warning("PacketIn msg too small");
      return;
    }

    const EnetFrame *frame = Interpret_cast<EnetFrame>(data.data());

    MatchBuilder match;
    match.addUnchecked(OFB_IN_PORT{msg->inPort()});
    match.addUnchecked(OFB_ETH_DST{frame->dst});
    match.addUnchecked(OFB_ETH_SRC{frame->src});

    ActionList actions;
    actions.add(AT_OUTPUT{msg->inPort()});

    InstructionList instructions;
    instructions.add(IT_APPLY_ACTIONS{&actions});

    FlowModBuilder flowMod;
    flowMod.setMatch(match);
    flowMod.setInstructions(instructions);
    flowMod.setBufferId(msg->bufferId());
    flowMod.send(message->source());
  }

  static ChannelListener *Factory() { return new CBenchReplyController; }
};

int main(int argc, char **argv) {
  log::configure(log::Level::Info);

  Driver driver;
  driver.installSignalHandlers();

  std::error_code err;
  (void)driver.listen(ChannelOptions::FEATURES_REQ, 0,
                      IPv6Endpoint{OFPGetDefaultPort()}, ProtocolVersions::All,
                      CBenchReplyController::Factory, err);

  if (err) {
    log_error("cbenchreply controller: driver.listen failed:", err);
    return 1;
  }

  driver.run();

  return 0;
}
