// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_SETERROR_H_
#define OFP_YAML_SETERROR_H_

#include "ofp/yaml/yllvm.h"

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

/// Return true if input io has an error.
///
/// This is a kludge (downcast). We need to know if the io object encountered
/// an error but the IO class doesn't support this. We need to reach into the
/// Input subclass to check for the error. (FIXME: bfish)
inline bool ErrorFound(llvm::yaml::IO &io) {
  assert(!io.outputting());
  llvm::yaml::Input *yin = static_cast<llvm::yaml::Input *>(&io);
  return static_cast<bool>(yin->error());
}

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_SETERROR_H_
