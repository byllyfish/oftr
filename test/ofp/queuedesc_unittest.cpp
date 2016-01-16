// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/queuedesc.h"
#include "ofp/queuedescproperty.h"

using namespace ofp;

TEST(queuedesc, test) {
  QueueDescBuilder queue;

  queue.setQueueId(0x1111111F);
  queue.setPort(0x2222222F);

  PropertyList props;
  props.add(QueueDescPropertyMinRate{34});
  props.add(QueueDescPropertyMaxRate{100});
  queue.setProperties(props);

  MemoryChannel channel{OFP_VERSION_5};
  queue.write(&channel);
  channel.flush();

  EXPECT_HEX("2222222F1111111F002000000000000000010008002200000002000800640000",
             channel.data(), channel.size());
}
