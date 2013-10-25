//  ===== ---- ofp/datapathid.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the DatapathID class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DATAPATHID_H
#define OFP_DATAPATHID_H

#include "ofp/types.h"
#include "ofp/enetaddress.h"
#include "ofp/byteorder.h"
#include <array>

namespace ofp { // <namespace ofp>

class DatapathID {
public:
  enum {
    Length = 8
  };

  using ArrayType = std::array<UInt8, Length>;

  DatapathID() : dpid_{} {}

  DatapathID(const ArrayType dpid) { dpid_ = dpid; }

  DatapathID(Big16 implementerDefined, EnetAddress macAddress);

  Big16 implementerDefined() const;
  EnetAddress macAddress() const;
  std::string toString() const;

  bool parse(const std::string &s);

  bool operator<(const DatapathID &rhs) const { return dpid_ < rhs.dpid_; }

  bool operator>(const DatapathID &rhs) const { return dpid_ > rhs.dpid_; }

  bool operator==(const DatapathID &rhs) const { return dpid_ == rhs.dpid_; }

  bool operator!=(const DatapathID &rhs) const { return !operator==(rhs); }

private:
  ArrayType dpid_;
};

std::ostream &operator<<(std::ostream &os, const DatapathID &value);

inline std::ostream &operator<<(std::ostream &os, const DatapathID &value) {
  return os << value.toString();
}

} // </namespace ofp>

#endif // OFP_DATAPATHID_H
