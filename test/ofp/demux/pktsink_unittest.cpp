// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/demux/pktsink.h"

#include "ofp/unittest.h"

using namespace ofp;

TEST(pktsink, test) {
  demux::PktSink sink;
  ByteList data{HexToRawData("0000000000010000000000020800")};
  Timestamp now = Timestamp::now();

  if (sink.openFile("/tmp/pktsink_unittest.pcap")) {
    for (unsigned i = 0; i < 10; ++i) {
      Timestamp ts{now.seconds(), now.nanoseconds() + i + 1000 * i};
      sink.write(ts, data.toRange(), 1000);
    }
  }
}

TEST(pktsink, close) {
  demux::PktSink sink;
  EXPECT_EQ("", sink.error());

  // close() is a no-op if file not open.
  sink.close();
  EXPECT_EQ("", sink.error());
}
