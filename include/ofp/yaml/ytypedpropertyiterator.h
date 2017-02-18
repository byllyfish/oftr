// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YTYPEDPROPERTYITERATOR_H_
#define OFP_YAML_YTYPEDPROPERTYITERATOR_H_

#include "ofp/propertyrange.h"

namespace ofp {
namespace detail {

template <class ItemType>
class TypedPropertyIterator {
 public:
  explicit TypedPropertyIterator(PropertyIterator iter) : iter_{iter} {}

  ItemType &operator*() {
    return llvm::yaml::Ref_cast<ItemType>(RemoveConst_cast(*iter_));
  }
  PropertyIterator::pointer operator->() const { return iter_.operator->(); }
  bool operator<(const TypedPropertyIterator &rhs) const {
    return iter_ < rhs.iter_;
  }
  void operator++() { ++iter_; }

 private:
  PropertyIterator iter_;
};

struct OptionalPropertyList {
  // Need begin/end so mapOptional("properties", ...) will compile.
  // N.B. This code is never actually called.
  constexpr int begin() const { return 0; }
  constexpr int end() const { return 0; }
};

}  // namespace detail
}  // namespace ofp

#endif  // OFP_YAML_YTYPEDPROPERTYITERATOR_H_
