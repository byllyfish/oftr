//  ===== ---- ofp/apiencoder_unittest.cpp -----------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for ApiEncoder class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/yaml/apiencoder.h"

using namespace ofp::yaml;

TEST(apiencoder, dumptokens)
{
	const char *hex = "2D2D2D0A6576656E743A204C49424F46505F4C495354454E5F5245504C590A6D73673A0A2020706F727420393008080A2D2D2D0A";
	std::string data = ofp::HexToRawData(hex);

	std::string output;
    llvm::raw_string_ostream outStream(output);
	
	bool result = llvm::yaml::dumpTokens(data, outStream);
	EXPECT_FALSE(result);
	
	const char *expected = R"""(Stream-Start: 
Document-Start: ---
Block-Mapping-Start: 
Key: event
Scalar: event
Value: :
Scalar: LIBOFP_LISTEN_REPLY
Key: msg
Scalar: msg
Value: :

)""";

	EXPECT_EQ(expected, outStream.str());
}

TEST(apiencoder, test) 
{
	const char *hex = "2D2D2D0A6576656E743A204C49424F46505F4C495354454E5F5245504C590A6D73673A0A2020706F727420393008080A2D2D2D0A";
	std::string data = ofp::HexToRawData(hex);

	// FIXME!  This test is disabled. It causes a crash in yaml-io. 
	// I've added code to strip out control-chars if that's the problem?
	// 
	// Pass connection == nullptr for testing.
#define OFP_TEST_NOT_WORKING 1
#if !OFP_TEST_NOT_WORKING
	ApiEncoder encoder(data, nullptr);
#endif
#undef OFP_TEST_NOT_WORKING
}



