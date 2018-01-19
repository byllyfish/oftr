// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_PROPERTYLIST_H_
#define OFP_PROPERTYLIST_H_

#include "ofp/propertyrange.h"
#include "ofp/protocollist.h"
#include "ofp/queueproperty.h"
#include "ofp/tablefeatureproperty.h"

namespace ofp {

namespace detail {

template <class T>
struct has_FixedHeaderSize {
  template <typename U>
  static char test(decltype(U::FixedHeaderSize) *);

  template <typename U>
  static double test(...);

  static bool const value = (sizeof(test<T>(0)) == 1);
};

}  // namespace detail

class PropertyList : public ProtocolList<PropertyRange> {
  using Inherited = ProtocolList<PropertyRange>;

 public:
  using Inherited::Inherited;

  template <class PropertyType>
  EnableIf<detail::has_FixedHeaderSize<PropertyType>::value, void> add(
      const PropertyType &property) {
    assert((buf_.size() % 8) == 0);

    ByteRange value = property.valueRef();
    size_t propSize = PropertyType::FixedHeaderSize + value.size();
    buf_.add(&property, PropertyType::FixedHeaderSize);
    buf_.add(value.data(), value.size());
    buf_.addZeros(PadLength(propSize) - propSize);

    assert(property.size() == propSize);
    assert((buf_.size() % 8) == 0);
  }

  template <class PropertyType>
  EnableIf<!detail::has_FixedHeaderSize<PropertyType>::value, void> add(
      const PropertyType &property) {
    static_assert((sizeof(property) % 8) == 0, "Expected multiple of 8.");
    buf_.add(&property, sizeof(property));
  }
};

}  // namespace ofp

#endif  // OFP_PROPERTYLIST_H_
