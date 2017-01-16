// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_MATCHBUILDER_H_
#define OFP_MATCHBUILDER_H_

#include "ofp/constants.h"
#include "ofp/match.h"
#include "ofp/oxmfields.h"
#include "ofp/oxmlist.h"
#include "ofp/prerequisites.h"

namespace ofp {

class MatchBuilder {
 public:
  MatchBuilder() = default;
  /* implicit NOLINT */ MatchBuilder(const Match &match)
      : list_{match.toRange()} {}

  const UInt8 *data() const { return list_.data(); }
  size_t size() const { return list_.size(); }

  OXMIterator begin() const { return list_.begin(); }
  OXMIterator end() const { return list_.end(); }

  template <class ValueType>
  void add(ValueType value) {
    Prerequisites::insertAll(&list_, ValueType::prerequisites());
    if (!Prerequisites::substitute(&list_, ValueType::type(), &value,
                                   sizeof(value))) {
      list_.add(value);
    }
  }

  template <class ValueType>
  void add(ValueType value, ValueType mask) {
    assert(ValueType::maskSupported());
    Prerequisites::insertAll(&list_, ValueType::prerequisites());
    if (!Prerequisites::substitute(&list_, ValueType::type(), &value, &mask,
                                   sizeof(value))) {
      list_.add(value, mask);
    }
  }

  void addUnchecked(OXMType type, const ByteRange &data) {
    list_.add(type, data.data(), data.size());
  }

  void addUnchecked(OXMType type, Big32 experimenter, const ByteRange &data) {
    if (experimenter)
      list_.addExperimenter(type, experimenter, data.data(), data.size());
    else
      list_.add(type, data.data(), data.size());
  }

  void addUnchecked(OXMType type, Big32 experimenter, const ByteRange &data,
                    const ByteRange &mask) {
    assert(data.size() == mask.size());
    if (experimenter)
      list_.addExperimenter(type, experimenter, data.data(), mask.data(),
                            data.size());
    else
      list_.add(type, data.data(), mask.data(), data.size());
  }

  template <class ValueType>
  void addUnchecked(ValueType value) {
    list_.add(value);
  }

  template <class ValueType>
  void addOrderedUnchecked(ValueType value) {
    static_assert(sizeof(value) < 256, "oxm_length must be <= 255.");
    list_.addOrdered(ValueType::type(), &value, sizeof(value));
  }

  template <class ValueType>
  void addUnchecked(ValueType value, ValueType mask) {
    list_.add(value, mask);
  }

  template <class ValueType>
  void replaceUnchecked(ValueType value) {
    list_.replace(value);
  }

  void clear() { list_.clear(); }

  bool validate() const {
    Prerequisites::FailureReason ignore;
    return validate(&ignore);
  }

  bool validate(Prerequisites::FailureReason *reason) const {
    if (!Prerequisites::checkAll(list_.toRange(), reason)) {
      return false;
    }
    return true;
  }

  OXMRange toRange() const { return list_.toRange(); }

 private:
  OXMList list_;
};

}  // namespace ofp

#endif  // OFP_MATCHBUILDER_H_
