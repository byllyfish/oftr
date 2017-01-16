// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PROPERTYRANGE_H_
#define OFP_PROPERTYRANGE_H_

#include "ofp/protocolrange.h"

namespace ofp {

namespace detail {

class PropertyIteratorItem : private NonCopyable {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(Big16),
    ProtocolIteratorAlignment = 8
  };

  UInt16 type() const { return type_; }
  UInt16 size() const { return len_; }

  template <class Type>
  const Type &property() const {
    return *Interpret_cast<Type>(this);
  }

  ByteRange value() const { return SafeByteRange(this, len_, 4); }

  bool validateInput(Validation *context) const { return true; }

 private:
  Big16 type_;
  Big16 len_;
};

}  // namespace detail

using PropertyIterator = ProtocolIterator<detail::PropertyIteratorItem,
                                          ProtocolIteratorType::Property>;

class PropertyRange : public ProtocolRange<PropertyIterator> {
  using Inherited = ProtocolRange<PropertyIterator>;

 public:
  using Inherited::Inherited;

  /// \return Iterator to first property found with specified type.
  Iterator findProperty(UInt16 type) const {
    for (auto iter = begin(); iter < end(); ++iter) {
      if (iter->type() == type)
        return iter;
    }
    return end();
  }

  /// \return value of property, when size of property is fixed.
  ///
  /// `PropertyType` must implement the `ValueType` concept.
  template <class PropertyType,
            class ValueType = typename PropertyType::ValueType>
  ValueType value() const {
    auto iter = findProperty(PropertyType::type());
    if (iter != end() && iter.size() == sizeof(PropertyType)) {
      return iter->template property<PropertyType>().value();
    }
    return PropertyType::defaultValue();
  }

  /// Get value of property, when size of property is fixed.
  ///
  /// \return false if property is not present
  ///
  /// `PropertyType` must implement the `ValueType` concept.
  template <class PropertyType,
            class ValueType = typename PropertyType::ValueType>
  bool value(ValueType *val) const {
    auto iter = findProperty(PropertyType::type());
    if (iter != end() && iter.size() == sizeof(PropertyType)) {
      *val = iter->template property<PropertyType>().value();
      return true;
    }
    return false;
  }

  /// \return value of property, when it is a range (and size is variable)
  template <class PropertyType,
            class ValueType = typename PropertyType::ValueType>
  ValueType valueRange(
      const ValueType &defaultValue = PropertyType::defaultValue()) const {
    auto iter = findProperty(PropertyType::type());
    if (iter != end() && iter.size() >= 4) {
      return iter->template property<PropertyType>().value();
    }
    return defaultValue;
  }
};

}  // namespace ofp

#endif  // OFP_PROPERTYRANGE_H_
