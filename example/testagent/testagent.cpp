// Copyright 2014-present Bill Fisher. All rights reserved.

#include "testagent.h"
#include "ofp/yaml/decoder.h"
#include <iostream>

using namespace testagent;

void TestAgent::onMessage(const Message *message) {
  bool writeNow = false;

  if (message->type() == OFPT_FEATURES_REQUEST) {
    FeaturesReplyBuilder reply{message->xid()};
    reply.setDatapathId(DatapathID{"12-34-56-78-9A-BC-CD-EF"});
    reply.send(message->source());
    writeNow = true;

  } else if (message->type() == OFPT_BARRIER_REQUEST) {
    BarrierReplyBuilder reply{message};
    reply.send(message->source());

  } else if (message->isRequestType()) {
    ErrorBuilder error{message->xid()};
    error.setErrorType(OFPET_BAD_REQUEST);
    error.setErrorCode(OFPBRC_BAD_TYPE);
    error.setErrorData(message);
    error.send(message->source());
  }

  // Write YAML to stdout.
  yaml::Decoder decoder{message};
  if (decoder.error().empty()) {
    std::cout << decoder.result() << std::flush;
  } else {
    std::cout << "ERROR: " << decoder.error() << '['
              << RawDataToHex(message->data(), message->size()) << ']'
              << std::endl;
  }

  if (writeNow) {
    // As soon as we've sent the features reply, write all messages.
    writeMessages(message->source());
  }
}

void TestAgent::writeMessages(Channel *channel) {
  // Read binary messages from input file and write them to the channel.

  while (true) {
    char buf[1000];
    std::cin.read(buf, sizeof(buf));

    if (std::cin) {
      channel->write(buf, sizeof(buf));
    } else {
      channel->write(buf, Unsigned_cast(std::cin.gcount()));
      break;
    }
  }

  channel->flush();
  channel->driver()->stop(500_ms);
}
