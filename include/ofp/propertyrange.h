#ifndef OFP_PROPERTYRANGE_H_
#define OFP_PROPERTYRANGE_H_

#include "ofp/protocolrange.h"

namespace ofp {

namespace detail {

class PropertyIteratorItem : private NonCopyable {
 public:
  enum {
    ProtocolIteratorSizeOffset = sizeof(Big16)
  };

  UInt16 type() const { return type_; }
  UInt16 size() const { return len_; }

  template <class Type>
  const Type &property() const {
    return *reinterpret_cast<const Type *>(this);
  }

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

  template <class PropertyType>
  Iterator findProperty() const {
    for (auto iter = begin(); iter < end(); ++iter) {
      if (iter->type() == PropertyType::type() &&
          iter->size() == sizeof(PropertyType)) {
        return iter;
      }
    }
    return end();
  }

  template <class PropertyType>
  typename PropertyType::ValueType value() const {
    auto iter = findProperty<PropertyType>();
    if (iter != end()) {
      return iter->template property<PropertyType>().value();
    }
    return PropertyType::defaultValue();
  }
};

}  // namespace ofp

#endif  // OFP_PROPERTYRANGE_H_
