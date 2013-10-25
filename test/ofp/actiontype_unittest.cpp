//  ===== ---- ofp/actiontype_unittest.cpp -----------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for ActionType class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/actiontype.h"


using namespace ofp;


TEST(actiontype, AT_OUTPUT)
{
	ActionType type{OFPAT_OUTPUT, 8};

	EXPECT_EQ(4, sizeof(type));
	EXPECT_EQ(OFPAT_OUTPUT, type.type());
	EXPECT_EQ(8, type.length());
	EXPECT_EQ(0x08, type.nativeType());

	auto expected = HexToRawData("00000008");
	EXPECT_EQ(0, std::memcmp(expected.data(), &type, sizeof(type)));
}


TEST(actiontype, AT_EXPERIMENTER)
{
	ActionType type{OFPAT_EXPERIMENTER, 64};

	EXPECT_EQ(4, sizeof(type));
	EXPECT_EQ(OFPAT_EXPERIMENTER, type.type());
	EXPECT_EQ(64, type.length());
	EXPECT_EQ(0xFFFF0040, type.nativeType());
	
	auto expected = HexToRawData("FFFF0040");
	EXPECT_EQ(0, std::memcmp(expected.data(), &type, sizeof(type)));
}

TEST(actiontype, AT_EXPERIMENTER_2)
{
	ActionType type{OFPAT_EXPERIMENTER, 65530};

	EXPECT_EQ(4, sizeof(type));
	EXPECT_EQ(OFPAT_EXPERIMENTER, type.type());
	EXPECT_EQ(65530, type.length());
	EXPECT_EQ(0xFFFFFFFA, type.nativeType());
	
	auto expected = HexToRawData("FFFFFFFA");
	EXPECT_EQ(0, std::memcmp(expected.data(), &type, sizeof(type)));
}

