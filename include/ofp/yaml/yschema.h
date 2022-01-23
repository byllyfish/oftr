// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YSCHEMA_H_
#define OFP_YAML_YSCHEMA_H_

#include <set>

#include "ofp/yaml/yllvm.h"

namespace ofp {
namespace yaml {

OFP_BEGIN_IGNORE_PADDING

class Schema {
 public:
  explicit Schema(const std::string &schema) : buf_{schema} {
    // Make sure schema ends with a newline.
    if (!buf_.empty() && buf_.back() != '\n')
      buf_.push_back('\n');
    init(buf_.c_str());
  }

  llvm::StringRef type() const { return type_; }
  llvm::StringRef name() const { return name_; }
  llvm::StringRef value() const { return value_; }

  bool equals(llvm::StringRef s) const;

  std::set<std::string> dependsOnSchemas() const;

  void print(std::ostream &os, bool brief = false) const;
  void printValue(std::ostream &os, unsigned indent = 0) const;

 private:
  std::string buf_;
  llvm::StringRef type_;
  llvm::StringRef name_;
  llvm::StringRef value_;
  bool isObject_ = false;

  void init(const char *schema);

  static std::string MakeSchemaString(
      const char *name, const std::vector<llvm::StringRef> &values,
      size_t size);

  static std::string MakeFlagSchemaString(const char *name,
                                          const std::string &values,
                                          size_t size);

  template <class Type>
  friend std::string MakeSchema(const char *name);

  template <class Type>
  friend std::string MakeFlagSchema(const char *name);
};

OFP_END_IGNORE_PADDING

typedef std::string (*SchemaMakerFunction)(const char *);

template <class Type>
std::string MakeSchema(const char *name) {
  auto values = llvm::yaml::ScalarTraits<Type>::converter.listAll();
  return Schema::MakeSchemaString(name, values, sizeof(Type));
}

template <class Type>
std::string MakeFlagSchema(const char *name) {
  return Schema::MakeFlagSchemaString(name, AllFlags<Type>(), sizeof(Type));
}

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_YSCHEMA_H_
