// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/unittest.h"
#include "ofp/tablestatus.h"
#include "ofp/tablemodproperty.h"

using namespace ofp;

const OFPTableStatusReason kFakeReason =
    static_cast<OFPTableStatusReason>(0x44);

TEST(tablestatus, test) {
  TableDescBuilder table;
  table.setTableId(0x11);

  TableModPropertyEviction eviction;
  eviction.setFlags(0x31323334);

  TableModPropertyVacancy vacancy;
  vacancy.setVacancyDown(0x41);
  vacancy.setVacancyUp(0x51);
  vacancy.setVacancy(0x61);

  PropertyList properties;
  properties.add(eviction);
  properties.add(vacancy);
  table.setProperties(properties);

  TableStatusBuilder builder;
  builder.setReason(kFakeReason);
  builder.setTable(table);

  MemoryChannel channel{OFP_VERSION_5};
  UInt32 xid = builder.send(&channel);

  EXPECT_EQ(1, xid);

  EXPECT_EQ(40, channel.size());
  EXPECT_HEX(
      "051F00280000000144000000000000000018110000000000000200083132333400030008"
      "41516100",
      channel.data(), channel.size());

  Message message{channel.data(), channel.size()};
  message.transmogrify();

  const TableStatus *msg = TableStatus::cast(&message);
  EXPECT_TRUE(msg);

  if (msg) {
    EXPECT_EQ(kFakeReason, msg->reason());

    const TableDesc &p = msg->table();
    EXPECT_EQ(0x11, p.tableId());
  }
}
