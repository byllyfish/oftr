// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/queue.h"
#include "ofp/queuelist.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(queue, test) {
  QueueBuilder queue;

  queue.setQueueId(0x11111111);
  queue.setPort(0x22222222);

  PropertyList props;
  props.add(QueuePropertyMinRate{34});
  props.add(QueuePropertyMaxRate{100});
  queue.setProperties(props.toRange());

  QueueList list;
  list.add(queue);

  for (auto &iter : list) {
    EXPECT_EQ(0x11111111, iter.queueId());
    EXPECT_EQ(0x22222222, iter.port());

    EXPECT_EQ(34, iter.properties().value<QueuePropertyMinRate>());
    EXPECT_EQ(100, iter.properties().value<QueuePropertyMaxRate>());
    EXPECT_EQ(2, iter.properties().itemCount());
  }

  EXPECT_EQ(0x30, list.size());
  EXPECT_HEX(
      "111111112222222200300000000000000001001000000000002200000000000000020010"
      "000000000064000000000000",
      list.data(), list.size());
}

TEST(queue, queuepropertyexperimenter) {
  {
    PropertyList props;
    props.add(QueuePropertyExperimenter{0x1234, {"foo", 3}});

    EXPECT_EQ(24, props.size());
    EXPECT_HEX("FFFF0013000000000000123400000000666F6F0000000000", props.data(),
               props.size());
  }

  {
    PropertyList props;
    props.add(QueuePropertyExperimenter{0x12345678, {}});

    EXPECT_EQ(16, props.size());
    EXPECT_HEX("FFFF0010000000001234567800000000", props.data(), props.size());
  }
}
