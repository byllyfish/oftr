// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YTYPEDPROPERTYITERATOR_H_
#define OFP_YAML_YTYPEDPROPERTYITERATOR_H_

#include "ofp/propertyrange.h"

namespace ofp {
namespace detail {

template <class ItemType>
class TypedPropertyIterator {
 public:
  TypedPropertyIterator(PropertyIterator iter) : iter_{iter} {}

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

}  // namespace detail
}  // namespace ofp

#endif  // OFP_YAML_YTYPEDPROPERTYITERATOR_H_
