//  ===== ---- ofp/bucket.h --------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Bucket class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_BUCKET_H
#define OFP_BUCKET_H

#include "ofp/byteorder.h"
#include "ofp/actionrange.h"

namespace ofp { // <namespace ofp>

class Bucket {
public:
	explicit Bucket(const ActionRange &actions);

	UInt16 weight() const { return weight_; }
	void setWeight(UInt16 weight) { weight_ = weight; }

	UInt32 watchPort() const { return watchPort_; }
	void setWatchPort(UInt32 watchPort) { watchPort_ = watchPort; }

	UInt32 watchGroup() const { return watchGroup_; }
	void setWatchGroup(UInt32 watchGroup) { watchGroup_ = watchGroup; }

	ActionRange actions() const { return actions_; }

private:
	Big16 len_;
	Big16 weight_ = 0;
	Big32 watchPort_ = 0;
	Big32 watchGroup_ = 0;
	Padding<4> pad_;
	ActionRange actions_;

	enum {
		SizeWithoutActionRange = 16
	};

	friend class BucketList;
};

static_assert(sizeof(Bucket) - sizeof(ActionRange) == 16, "Unexpected size.");
static_assert(IsStandardLayout<Bucket>(), "Expected standard layout.");
static_assert(IsTriviallyCopyable<Bucket>(), "Expected trivially copyable.");

} // </namespace ofp>

#endif // OFP_BUCKET_H
