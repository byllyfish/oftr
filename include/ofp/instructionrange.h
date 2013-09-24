//  ===== ---- ofp/instructionrange.h ----------------------*- C++ -*- =====  //
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
/// \brief Defines the InstructionRange class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONRANGE_H
#define OFP_INSTRUCTIONRANGE_H

#include "ofp/byterange.h"
#include "ofp/instructioniterator.h"

namespace ofp { // <namespace ofp>

class InstructionRange {
public:

    InstructionRange(const ByteRange &range) : range_{range} {}
    
   /// \returns number of items in the range.
    size_t itemCount() const
    {
        return InstructionIterator::distance(begin(), end());
    }

    InstructionIterator begin() const
    {
        return InstructionIterator{range_.begin()};
    }

    InstructionIterator end() const
    {
        return InstructionIterator{range_.end()};
    }

    const UInt8 *data() const { return range_.data(); }
    size_t size() const { return range_.size(); }
    
private:
	ByteRange range_;
};

} // </namespace ofp>

#endif // OFP_INSTRUCTIONRANGE_H
