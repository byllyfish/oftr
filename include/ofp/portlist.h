//  ===== ---- ofp/portlist.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the PortList class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_PORTLIST_H_
#define OFP_PORTLIST_H_

#include "ofp/bytelist.h"
#include "ofp/port.h"
#include "ofp/portrange.h"

namespace ofp {

class PortList {
public:
  PortList() = default;

  const UInt8 *data() const { return buf_.data(); }
  size_t size() const { return buf_.size(); }
  
  void add(const Port &port) { buf_.add(&port, sizeof(Port)); }
  void add(const PortBuilder &port) { add(port.toPort()); }

  PortRange toRange() const { return buf_.toRange(); }

  void operator=(const PortRange &range) { buf_ = range.toByteRange(); }

private:
  ByteList buf_;
};

}  // namespace ofp

#endif  // OFP_PORTLIST_H_
