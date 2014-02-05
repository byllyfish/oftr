//  ===== ---- ofp/oxmtypeset.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the OXMTypeSet class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_OXMTYPESET_H_
#define OFP_OXMTYPESET_H_

#include <bitset>
#include "ofp/oxmtype.h"

namespace ofp {

OFP_BEGIN_IGNORE_PADDING

class OXMTypeSet {
public:
  OXMTypeSet() {}

  bool find(OXMType type);
  bool add(OXMType type);

private:
  enum {
    MaxOXMClasses = 10
  };

  struct ClassEntry {
    std::bitset<128> fields;
    UInt16 oxmClass;
  };

  // TODO(bfish): replace with std::array?
  ClassEntry classes_[MaxOXMClasses];
  unsigned classCount_ = 0;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_OXMTYPESET_H_
