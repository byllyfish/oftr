#include "testagent.h"
#include "ofp/yaml/decoder.h"
//#include "ofp/api/apiserver.h"
#include <iostream>

using namespace testagent;

const UInt32 TIMER_START_API_INPUT = 1;

void TestAgent::onChannelUp(Channel *channel) {
  assert(channel_ == nullptr);

  channel_ = channel;
  channel_->scheduleTimer(TIMER_START_API_INPUT, 1000_ms);
}

void TestAgent::onChannelDown(Channel *channel) {
  assert(channel_ == channel);

  //apiServer_.reset(nullptr);
  channel->driver()->stop();
}

void TestAgent::onMessage(const Message *message) {
  if (message->type() == OFPT_FEATURES_REQUEST) {
    FeaturesReplyBuilder reply{message->xid()};
    reply.setDatapathId(DatapathID{"12-34-56-78-9A-BC-CD-EF"});
    reply.send(message->source());
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
}

void TestAgent::onTimer(UInt32 timerID) {
  // When timer expires, set up api server and start reading YAML input.

  if (timerID == TIMER_START_API_INPUT) {
    startApiInput();
  }

#if 0
	 else if (timerID == TIMER_STOP_API_INPUT) {
		channel_->driver()->stop();
	} else if (timerID == 0xFFFFFFFFU) {
		channel_->scheduleTimer(TIMER_STOP_API_INPUT, 1500_ms);
	}
#endif
}

void TestAgent::startApiInput() {
  // Read binary messages from input file and write them to the channel.
  
  while (true) {
    char buf[1000];
    std::cin.read(buf, sizeof(buf));

    if (std::cin) {
      channel_->write(buf, sizeof(buf));
    } else {
      channel_->write(buf, Unsigned_cast(std::cin.gcount()));
      break;
    }
  }

  channel_->flush();

  channel_->driver()->stop(1000_ms);

  //apiServer_ = std::unique_ptr<api::ApiServer>{
  //    new api::ApiServer{channel_->driver(), 0, -1, channel_}};
}
