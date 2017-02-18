// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
  enum { MaxOXMClasses = 10 };

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
