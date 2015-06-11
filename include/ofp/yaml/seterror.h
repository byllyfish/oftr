#ifndef OFP_YAML_SETERROR_H_
#define OFP_YAML_SETERROR_H_

namespace ofp {
namespace yaml {

/// Set an error message on the current YAML input when the enum `name` is not 
/// among `vals`.
llvm::StringRef SetEnumError(void *ctxt, llvm::StringRef name, const std::vector<llvm::StringRef> &vals);

}  // namespace yaml
}  // namespace ofp

#endif // OFP_YAML_SETERROR_H_
