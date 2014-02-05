//  ===== ---- ofp/writable.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the abstract Writable class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_WRITABLE_H_
#define OFP_WRITABLE_H_

#include "ofp/types.h"
#include "ofp/padding.h"

namespace ofp {

class Writable {
public:
  virtual ~Writable() {}

  virtual UInt8 version() const = 0;
  virtual UInt32 nextXid() = 0;

  virtual void write(const void *data, size_t length) = 0;
  virtual void flush() = 0;

  void write(const void *data, size_t length, size_t padSize) {
    assert(padSize < 8);
  	Padding<8> pad;
  	write(data, length);
  	write(&pad, padSize);
  }
};

}  // namespace ofp

#endif  // OFP_WRITABLE_H_
