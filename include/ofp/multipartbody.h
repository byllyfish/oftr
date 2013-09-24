//  ===== ---- ofp/multipartbody.h -------------------------*- C++ -*- =====  //
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
/// \brief Defines the MultipartBody and MultipartBodyBuilder. 
//  ===== ------------------------------------------------------------ =====  //

// FIXME - I don't think this class is necessary anymore????

#ifndef OFP_MULTIPARTBODY_H
#define OFP_MULTIPARTBODY_H

#include "ofp/bytelist.h"
#include "ofp/constants.h"

namespace ofp { // <namespace ofp>

class MultipartBody {
public:


private:
	
};


template <class ValueType>
class MultipartBodyBuilder {
public:

	void add(const ValueType &value) { body_.add(&value, sizeof(ValueType)); }

private:
	ByteList body_;
};

} // </namespace ofp>

#endif // OFP_MULTIPARTBODY_H
