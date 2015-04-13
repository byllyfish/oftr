#ifndef OFP_YAML_YSCHEMA_H_
#define OFP_YAML_YSCHEMA_H_

#include "ofp/yaml/yllvm.h"

namespace ofp {
namespace yaml {

class Schema {
public:
  explicit Schema(const char *const schema);

  llvm::StringRef type() const { return type_; }
  llvm::StringRef name() const { return name_; }
  llvm::StringRef value() const { return value_; }

  bool equals(llvm::StringRef s) const;

private:
  llvm::StringRef type_;
  llvm::StringRef name_;
  llvm::StringRef value_;
  bool isObject_ = false;
};

}  // namespace yaml
}  // namespace ofp

#endif // OFP_YAML_YSCHEMA_H_
