//  ===== ---- ofp/oxmtypeset.cpp --------------------------*- C++ -*- =====  //
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
/// \brief Implements OXMTypeSet class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/oxmtypeset.h"
#include "ofp/log.h"

using namespace ofp;

bool OXMTypeSet::find(OXMType type)
{
	UInt16 oxmClass = type.oxmClass();
	UInt8 oxmField = type.oxmField();

	assert(oxmField < 128);

	for (unsigned i = 0; i < classCount_; ++i) {
		if (classes_[i].oxmClass == oxmClass) {
			return classes_[i].fields.test(oxmField);
		}
	}
	return false;
}


bool OXMTypeSet::add(OXMType type)
{
	UInt16 oxmClass = type.oxmClass();
	UInt8 oxmField = type.oxmField();

	assert(oxmField < 128);

	for (unsigned i = 0; i < classCount_; ++i) {
		if (classes_[i].oxmClass == oxmClass) {
			if (!classes_[i].fields.test(oxmField)) {
				classes_[i].fields.set(oxmField);
				return true;
			}
			return false;
		}
	}

	if (classCount_ < MaxOXMClasses) {
		classes_[classCount_].oxmClass = oxmClass;
		classes_[classCount_].fields.set(oxmField);
		++classCount_;
	} else {
		log::info("Number of OXMTypeSet classes exceeded: ", MaxOXMClasses);
	}

	return true;
}
