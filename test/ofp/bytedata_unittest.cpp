//  ===== ---- ofp/bytedata_unittest.cpp -------------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for ByteData class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/bytedata.h"

using namespace ofp;

TEST(bytedata, basics)
{
    ByteData b{"abc", 3};

    EXPECT_EQ(3, b.size());
    EXPECT_NE(nullptr, b.data());

    ByteData b2{std::move(b)};
    EXPECT_EQ(3, b2.size());

    // Check moved from object.
    EXPECT_EQ(nullptr, b.data());
    EXPECT_EQ(0, b.size());
}
