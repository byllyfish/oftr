#ifndef OFP_PROPERTYRANGE_H_
#define OFP_PROPERTYRANGE_H_

#include "ofp/protocolrange.h"

namespace ofp {

namespace detail {

class PropertyIteratorItem : private NonCopyable {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(Big16), ProtocolIteratorAlignment = 8
  };

  UInt16 type() const { return type_; }
  UInt16 size() const { return len_; }

  template <class Type>
  const Type &property() const {
    return *reinterpret_cast<const Type *>(this);
  }

  ByteRange value() const { return ByteRange{BytePtr(this) + 4, len_ - 4U }; }

 private:
  Big16 type_;
  Big16 len_;
};

}  // namespace detail

using PropertyIterator = ProtocolIterator<detail::PropertyIteratorItem>;

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
  template <class PropertyType, class ValueType=typename PropertyType::ValueType>
  ValueType value() const {
    auto iter = findProperty(PropertyType::type());
    if (iter != end() && iter.size() == sizeof(PropertyType)) {
      return iter->template property<PropertyType>().value();
    }
    return PropertyType::defaultValue();
  }

  /// \return value of property, when it is a range (and size is variable)
  template <class PropertyType, class ValueType=typename PropertyType::ValueType>
  ValueType valueRange(const ValueType &defaultValue=PropertyType::defaultValue()) const {
    auto iter = findProperty(PropertyType::type());
    if (iter != end() && iter.size() >= 4) {
      return iter->template property<PropertyType>().value();
    }
    return defaultValue;
  }
};

}  // namespace ofp

#endif  // OFP_PROPERTYRANGE_H_
