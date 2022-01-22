// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmfields.h"

#include "ofp/oxmlist.h"
#include "ofp/unittest.h"

using namespace ofp;

TEST(oxmfields, OFB_TCP_DST) {
  OFB_TCP_DST tcpdst{80};

  EXPECT_EQ(80, tcpdst);
  EXPECT_EQ(0x80001c02, OFB_TCP_DST::type().oxmNative());
  EXPECT_EQ(2, sizeof(OFB_TCP_DST));

  OXMType types[] = {OFB_ETH_TYPE::type(), OFB_ETH_TYPE::type(),
                     OFB_IP_PROTO::type()};
  unsigned i = 0;
  for (auto &x : *OFB_TCP_DST::prerequisites()) {
    ASSERT_GT(ArrayLength(types), i);
    if (i >= ArrayLength(types))
      break;

    EXPECT_EQ(types[i], x.type());
    ++i;
  }
}

TEST(oxmfields, OFB_IPV6_ND_SLL) {
  OFB_IPV6_ND_SLL ndsll{MacAddress{}};

  EXPECT_EQ(ndsll.value(), MacAddress{});

  auto iter = OFB_IPV6_ND_SLL::prerequisites()->begin();
  auto end = OFB_IPV6_ND_SLL::prerequisites()->end();

  EXPECT_NE(iter, end);
  EXPECT_EQ((*iter).type(), OFB_ETH_TYPE::type());
  EXPECT_EQ((*iter).value<OFB_ETH_TYPE>(), 0x86dd);
  ++iter;
  EXPECT_NE(iter, end);
  EXPECT_EQ((*iter).type(), OFB_IP_PROTO::type());
  EXPECT_EQ((*iter).value<OFB_IP_PROTO>(), 58);
  ++iter;
  EXPECT_NE(iter, end);
  EXPECT_EQ((*iter).type(), OFB_ICMPV6_TYPE::type());
  EXPECT_EQ((*iter).value<OFB_ICMPV6_TYPE>(), 135);
  ++iter;
  EXPECT_EQ(iter, end);

  OXMList list;
  list.add(OFB_ETH_TYPE{0x86dd});
  list.add(OFB_IP_PROTO{58});
  list.add(OFB_ICMPV6_TYPE{135});
  EXPECT_EQ(list.toRange(), *OFB_IPV6_ND_SLL::prerequisites());
}

TEST(oxmfields, NXM_NX_TCP_FLAGS) {
  NXM_NX_TCP_FLAGS tcpflags{0x0102};

  EXPECT_EQ(0x0102, tcpflags);
  EXPECT_EQ(0x00014402, NXM_NX_TCP_FLAGS::type().oxmNative());
  EXPECT_EQ(2, sizeof(NXM_NX_TCP_FLAGS));
}
