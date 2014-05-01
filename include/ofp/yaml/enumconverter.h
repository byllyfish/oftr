#ifndef OFP_YAML_ENUMCONVERTER_H_
#define OFP_YAML_ENUMCONVERTER_H_

#include "llvm/ADT/StringRef.h"

namespace ofp {
namespace yaml {

template <class Type, size_t N>
class EnumConverter {
 public:
  constexpr EnumConverter(const char *const (&names)[N]) : names_{names} {}

  bool convert(llvm::StringRef name, Type *value) {
    // Check for name match.
    for (size_t i = 0; i < N; ++i) {
      if (equals(name, i)) {
        *value = static_cast<Type>(i);
        return true;
      }
    }
    // If it doesn't match string, check for integer value.
    unsigned long long num;
    if (!llvm::getAsUnsignedInteger(name, 0, num)) {
        *value = static_cast<Type>(num);
        return true;
    }
    return false;
  }

  bool convert(Type value, const char **name) const {
    if (value < N && value >= 0) {
      *name = names_[value];
      return true;
    }
    return false;
  }

 private:
  using Name = const char *const;
  const Name *const names_;

  bool equals(llvm::StringRef name, size_t i) const {
    const char *p = name.data();
    const char *pend = p + name.size();
    const char *c = names_[i];
    while (p < pend) {
      if (std::toupper(*p) != std::toupper(*c)) {
        return false;
      }
      ++p;
      ++c;
    }
    return true;
  }
};

template <class Type, size_t N>
EnumConverter<Type, N> MakeEnumConverter(const char *const (&names)[N]) {
  return EnumConverter<Type, N>{names};
}

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_ENUMCONVERTER_H_
