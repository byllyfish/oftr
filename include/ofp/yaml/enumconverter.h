// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_ENUMCONVERTER_H_
#define OFP_YAML_ENUMCONVERTER_H_

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ArrayRef.h"

namespace ofp {
namespace yaml {
namespace detail {

/// Return max storable integer value based on type's size in bytes.
template <class Type>
constexpr uint64_t MaxIntValue() {
  return (sizeof(Type) == 1) ? 0xffu : (sizeof(Type) == 2)
                                           ? 0xffffu
                                           : (sizeof(Type) == 4)
                                                 ? 0xffffffffu
                                                 : (sizeof(Type) == 8)
                                                       ? 0xffffffffffffffffu
                                                       : 0;
}

}  // namespace detail

template <class Type>
bool ParseUnsignedInteger(llvm::StringRef name, Type *value) {
  unsigned long long num;
  if (!llvm::getAsUnsignedInteger(name, 0, num)) {
    static_assert(detail::MaxIntValue<Type>() > 0, "Unexpected type");
    if (num <= detail::MaxIntValue<Type>()) {
      *value = static_cast<Type>(num);
      return true;
    }
  }
  return false;
}

template <class Type>
class EnumConverter {
 public:
  explicit EnumConverter(llvm::ArrayRef<llvm::StringRef> names,
                llvm::StringRef maxIntName = "")
      : names_{names}, maxIntName_{maxIntName} {}

  bool convert(llvm::StringRef name, Type *value) {
    // Check for name match.
    for (size_t i = 0, len = names_.size(); i < len; ++i) {
      if (name.equals(names_[i])) {
        *value = static_cast<Type>(i);
        return true;
      }
    }
    if (!maxIntName_.empty() && name.equals(maxIntName_)) {
      *value = static_cast<Type>(detail::MaxIntValue<Type>());
      return true;
    }
    // If it doesn't match string, check for integer value.
    return ParseUnsignedInteger(name, value);
  }

  bool convert(Type value, llvm::StringRef *name) const {
    if (!maxIntName_.empty() && value == detail::MaxIntValue<Type>()) {
      *name = maxIntName_;
      return true;
    }
    if (value < names_.size() && value >= 0) {
      *name = names_[value];
      return true;
    }
    return false;
  }

 private:
  llvm::ArrayRef<llvm::StringRef> names_;
  llvm::StringRef maxIntName_;
};

template <class Type>
class EnumConverterSparse {
 public:
  using Entry = std::pair<Type, llvm::StringRef>;

  explicit EnumConverterSparse(llvm::ArrayRef<Entry> entries) : entries_{entries} {}

  bool convert(llvm::StringRef name, Type *value) {
    // Check for name match.
    for (size_t i = 0, len = entries_.size(); i < len; ++i) {
      if (name.equals(entries_[i].second)) {
        *value = entries_[i].first;
        return true;
      }
    }
    return false;
  }

  bool convert(Type value, llvm::StringRef *name) const {
    // Check for value match.
    for (size_t i = 0, len = entries_.size(); i < len; ++i) {
      if (value == entries_[i].first) {
        *name = entries_[i].second;
        return true;
      }
    }
    return false;
  }

 private:
  llvm::ArrayRef<Entry> entries_;
};

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_ENUMCONVERTER_H_
