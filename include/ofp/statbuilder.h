// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_STATBUILDER_H_
#define OFP_STATBUILDER_H_

#include "ofp/stat.h"

namespace ofp {

class StatBuilder {
 public:
  StatBuilder() = default;

  const UInt8 *data() const { return list_.data(); }
  size_t size() const { return list_.size(); }

  OXMIterator begin() const { return list_.begin(); }
  OXMIterator end() const { return list_.end(); }

  template <class ValueType>
  void add(ValueType value) {
    list_.add(value);
  }

  void add(OXMType type, Big32 experimenter, const ByteRange &data) {
    if (experimenter)
      list_.addExperimenter(type, experimenter, data.data(), data.size());
    else
      list_.add(type, data.data(), data.size());
  }

  void clear() { list_.clear(); }

  OXMRange toRange() const { return list_.toRange(); }

 private:
  OXMList list_;
};

}  // namespace ofp

#endif  // OFP_STATBUILDER_H_
