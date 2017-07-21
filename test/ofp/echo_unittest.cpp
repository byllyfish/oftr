#include "ofp/echoreply.h"
#include "ofp/echorequest.h"
#include "ofp/memorychannel.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(echo, test_keep_alive_request) {
  MemoryChannel channel{OFP_VERSION_4};
  EchoRequestBuilder builder{0};
  builder.setKeepAlive();
  builder.send(&channel);

  Message message{channel.data(), channel.size()};
  const EchoRequest *req = EchoRequest::cast(&message);
  ASSERT_TRUE(req);
  EXPECT_FALSE(req->isPassThru());

  message.mutableHeader()->setType(EchoReply::type());
  const EchoReply *reply = EchoReply::cast(&message);
  ASSERT_TRUE(reply);
  EXPECT_TRUE(reply->isKeepAlive());
}

TEST(echo, test_short_request) {
  MemoryChannel channel{OFP_VERSION_4};
  EchoRequestBuilder builder{0};
  builder.setEchoData("1234567", 7);
  builder.send(&channel);

  Message message{channel.data(), channel.size()};
  const EchoRequest *req = EchoRequest::cast(&message);
  ASSERT_TRUE(req);
  EXPECT_FALSE(req->isPassThru());

  message.mutableHeader()->setType(EchoReply::type());
  const EchoReply *reply = EchoReply::cast(&message);
  ASSERT_TRUE(reply);
  EXPECT_FALSE(reply->isKeepAlive());
}
