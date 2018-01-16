// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/tabledesc.h"
#include "ofp/tablemodproperty.h"
#include "ofp/unittest.h"

using namespace ofp;

const OFPTableConfigFlags kFakeFlags =
    static_cast<OFPTableConfigFlags>(0x21222324);

TEST(tabledesc, test) {
  TableDescBuilder tableBuilder;

  tableBuilder.setTableId(0x11);
  tableBuilder.setConfig(kFakeFlags);

  TableModPropertyEviction eviction;
  eviction.setFlags(0x31323334);

  TableModPropertyVacancy vacancy;
  vacancy.setVacancyDown(0x41);
  vacancy.setVacancyUp(0x51);
  vacancy.setVacancy(0x61);

  PropertyList properties;
  properties.add(eviction);
  properties.add(vacancy);
  tableBuilder.setProperties(properties);

  MemoryChannel channel{OFP_VERSION_5};
  tableBuilder.write(&channel);
  channel.flush();

  EXPECT_HEX("001811002122232400020008313233340003000841516100", channel.data(),
             channel.size());
}
