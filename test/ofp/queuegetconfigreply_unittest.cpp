// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/queuegetconfigreply.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(queuegetconfigreply, QueueGetConfigReplyBuilder_v4) {
  PropertyList props;
  props.add(QueuePropertyMinRate{0x5555});
  props.add(QueuePropertyMaxRate{0x6666});

  QueueBuilder q1;
  q1.setQueueId(0x33333333);
  q1.setPort(0x44444444);
  q1.setProperties(props);

  QueueList queues;
  queues.add(q1);

  QueueGetConfigReplyBuilder reply;
  reply.setPort(0x22222222);
  reply.setQueues(queues);

  MemoryChannel channel{OFP_VERSION_4};
  (void)reply.send(&channel);

  EXPECT_EQ(0x40, channel.size());
  EXPECT_HEX(
      "041700400000000122222222000000003333333344444444003000000000000000010010"
      "00000000555500000000000000020010000000006666000000000000",
      channel.data(), channel.size());
}
