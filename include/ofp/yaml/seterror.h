// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_SETERROR_H_
#define OFP_YAML_SETERROR_H_

namespace ofp {
namespace yaml {

/// Set an error message on the current YAML input when the enum `name` is not
/// among `vals`.
llvm::StringRef SetEnumError(void *ctxt, llvm::StringRef name,
                             const std::vector<llvm::StringRef> &vals);

/// Set an error message on the current YAML input when there's an unrecognized
/// flag `name`.
void SetFlagError(llvm::yaml::IO &io, llvm::StringRef name,
                  const std::string &flagSchema);

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_SETERROR_H_
