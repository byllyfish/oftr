// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/unittest.h"
#include "ofp/yaml/decoder.h"

using namespace ofp;
using namespace ofp::yaml;

static void testDecodeFail(const char *hex, const char *err) {
  auto s = HexToRawData(hex);

  MessageInfo msgInfo;
  Message msg{s.data(), s.size()};
  msg.setInfo(&msgInfo);
  msg.transmogrify();

  log::debug("msg=", msg);

  Decoder decoder{&msg};

  EXPECT_EQ(err, decoder.error());
  EXPECT_EQ("", decoder.result());
}

