//  ===== ---- ofp/bucketlist.cpp --------------------------*- C++ -*- =====  //
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
/// \brief Implements the BucketList class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/bucketlist.h"

using namespace ofp;


void BucketList::add(const BucketBuilder &bucket)
{
	ActionRange actions = bucket.actions();

	buf_.add(&bucket, BucketBuilder::SizeWithoutActionRange);
	buf_.add(actions.data(), actions.size());
}