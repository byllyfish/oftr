//  ===== ---- ofp/actionlist_unittest.cpp -----------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements unit tests for ActionList class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/actionlist.h"
#include "ofp/actions.h"

using namespace ofp;

TEST(actionlist, empty)
{
    ActionList list;

    EXPECT_EQ(0, list.size());
}

TEST(actionlist, one)
{
    ActionList list;

    list.add(AT_COPY_TTL_OUT{});

    EXPECT_EQ(sizeof(AT_COPY_TTL_OUT), list.size());

    AT_COPY_TTL_OUT expected;
    EXPECT_EQ(0, std::memcmp(&expected, list.data(), list.size()));
}

TEST(actionlist, two)
{
    ActionList list;

    list.add(AT_COPY_TTL_OUT{});
    list.add(AT_OUTPUT{255, 511});

    EXPECT_EQ(sizeof(AT_COPY_TTL_OUT) + sizeof(AT_OUTPUT), list.size());
}
