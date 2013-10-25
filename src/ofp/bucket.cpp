//  ===== ---- ofp/bucket.cpp ------------------------------*- C++ -*- =====  //
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
/// \brief Implements the Bucket class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/bucket.h"

using namespace ofp;


ActionRange Bucket::actions() const
{
	assert(len_ >= sizeof(Bucket));

	size_t actLen = len_ - sizeof(Bucket);
	return ActionRange{ByteRange{BytePtr(this) + sizeof(Bucket), actLen}};
}


void BucketBuilder::setActions(ActionRange actions)
{
	bkt_.len_ = UInt16_narrow_cast(sizeof(Bucket) + actions.size());
	actions_ = actions;
}