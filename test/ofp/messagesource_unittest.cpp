// Copyright (c) 2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/messagesource.h"
#include "ofp/demux/pktsource.h"
#include "ofp/message.h"
#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"

using namespace ofp;
using demux::PktSource;
using demux::MessageSource;

static void callback(Message *message, void *context) {
  message->transmogrify();

  ofp::yaml::Decoder decoder{message, false, true};

  if (decoder.error().empty())
    log::debug("callback\n", decoder.result());
  else
    log::debug("callback error:", decoder.error());
}

TEST(messagesource, test) {
  PktSource pkt;
  MessageSource msg(callback, nullptr);

  if (pkt.openFile("/Users/bfish/Downloads/"
                   "cap_single,3-ovsk,protocols=OpenFlow13-remote,port=6653."
                   "pcap",
                   "tcp")) {
    msg.runLoop(&pkt);

  } else {
    log::debug("PktSource error:", pkt.error());
  }
}
