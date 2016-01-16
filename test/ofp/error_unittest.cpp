// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/error.h"

using namespace ofp;

TEST(error, experimenter) {
  ErrorBuilder errorBuilder{0x11111111};
  errorBuilder.setErrorCode(OFPErrorCodeMake(OFPET_EXPERIMENTER, 1));

  MemoryChannel channel{OFP_VERSION_1};
  errorBuilder.send(&channel);

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_HEX("0101000C00000001FFFF0001", message.data(), message.size());

  const Error *error = Error::cast(&message);

  EXPECT_EQ(OFPET_EXPERIMENTER, error->errorType());
}

TEST(error, flowmod_failed_v1) {
  ErrorBuilder errorBuilder{0x11111111};
  errorBuilder.setErrorCode(OFPFMFC_TABLE_FULL);

  MemoryChannel channel{OFP_VERSION_1};
  errorBuilder.send(&channel);

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_HEX("0101000C0000000100030000", message.data(), message.size());

  const Error *error = Error::cast(&message);

  EXPECT_EQ(OFPET_FLOW_MOD_FAILED, error->errorType());
  EXPECT_EQ(OFPFMFC_TABLE_FULL, error->errorCode());
}

TEST(error, flowmod_failed_v4) {
  ErrorBuilder errorBuilder{0x11111111};
  errorBuilder.setErrorCode(OFPFMFC_TABLE_FULL);

  MemoryChannel channel{OFP_VERSION_4};
  errorBuilder.send(&channel);

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_HEX("0401000C0000000100050001", message.data(), message.size());

  const Error *error = Error::cast(&message);

  EXPECT_EQ(OFPET_FLOW_MOD_FAILED, error->errorType());
  EXPECT_EQ(OFPFMFC_TABLE_FULL, error->errorCode());
}

TEST(error, unsupported_order_v1) {
  ErrorBuilder errorBuilder{0x11111111};
  errorBuilder.setErrorCode(OFPFMFC_UNSUPPORTED);

  MemoryChannel channel{OFP_VERSION_1};
  errorBuilder.send(&channel);

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_HEX("0101000C0000000100030005", message.data(), message.size());

  const Error *error = Error::cast(&message);

  EXPECT_EQ(OFP_VERSION_1, error->msgHeader()->version());
  EXPECT_EQ(OFPET_FLOW_MOD_FAILED, error->errorType());
  EXPECT_EQ(OFPFMFC_UNSUPPORTED, error->errorCode());
}

TEST(error, unsupported_order_v4) {
  ErrorBuilder errorBuilder{0x11111111};
  errorBuilder.setErrorCode(OFPFMFC_UNSUPPORTED);

  MemoryChannel channel{OFP_VERSION_4};
  errorBuilder.send(&channel);

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_HEX("0401000C000000010002000B", message.data(), message.size());

  const Error *error = Error::cast(&message);

  EXPECT_EQ(OFP_VERSION_4, error->msgHeader()->version());
  EXPECT_EQ(OFPET_BAD_ACTION, error->errorType());
  EXPECT_EQ(OFPBAC_UNSUPPORTED_ORDER, error->errorCode());
}

TEST(error, unsupported_action_order_v4) {
  ErrorBuilder errorBuilder{0x11111111};
  errorBuilder.setErrorCode(OFPBAC_UNSUPPORTED_ORDER);

  MemoryChannel channel{OFP_VERSION_4};
  errorBuilder.send(&channel);

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_HEX("0401000C000000010002000B", message.data(), message.size());

  const Error *error = Error::cast(&message);

  EXPECT_EQ(OFP_VERSION_4, error->msgHeader()->version());
  EXPECT_EQ(OFPET_BAD_ACTION, error->errorType());
  EXPECT_EQ(OFPBAC_UNSUPPORTED_ORDER, error->errorCode());
}

TEST(error, hello_failed_v1) {
  std::string explanation = "Supported Versions: [1, 2, 3, 4]";

  ErrorBuilder errorBuilder{0x12341234};
  errorBuilder.setErrorCode(OFPHFC_INCOMPATIBLE);
  errorBuilder.setErrorData(explanation.data(), explanation.size());

  MemoryChannel channel{OFP_VERSION_1};
  errorBuilder.send(&channel);

  Message message{channel.data(), channel.size()};
  message.transmogrify();
  EXPECT_HEX(
      "0101002C0000000100000000537570706F727465642056657273696F6E733A205B312C20"
      "322C20332C20345D",
      message.data(), message.size());

  const Error *error = Error::cast(&message);

  EXPECT_EQ(OFPET_HELLO_FAILED, error->errorType());
  EXPECT_EQ(OFPHFC_INCOMPATIBLE, error->errorCode());
}
