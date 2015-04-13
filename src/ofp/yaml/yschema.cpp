#include "ofp/yaml/yschema.h"

using namespace ofp::yaml;

// Compare two strings case insensitively skipping non-alphanumeric chars.
static bool equalsAlnum(llvm::StringRef lhs, llvm::StringRef rhs) {
  size_t i = 0; 
  size_t j = 0;

  while (i < lhs.size() && j < rhs.size()) {
    if (!std::isalnum(lhs[i])) {
      ++i;
      continue;
    }
    if (!std::isalnum(rhs[j])) {
      ++j;
      continue;
    }
    if (std::tolower(lhs[i]) != std::tolower(rhs[j])) {
      return false;
    }
    ++i;
    ++j;
  }

  return i >= lhs.size() && j >= rhs.size();
}

Schema::Schema(const char *const schema) {
  // Split first line from the rest.
  auto p = llvm::StringRef{schema}.split('\n');

  // Set value to the rest.
  value_ = p.second;

  //  If the first line is enclosed in braces, we trim them and set isObject_ 
  //  to true.
  if (p.first.startswith("{")) {
    p.first = p.first.ltrim("{").rtrim("}");
    isObject_ = true;
  }

  // Parse the first line of form `type/name` or just `name`.
  std::tie(type_, name_) = p.first.split('/');
  if (name_.empty()) {
    std::swap(type_, name_);
  }
}

bool Schema::equals(llvm::StringRef s) const {
  return equalsAlnum(name_, s);
}
