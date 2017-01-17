// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_SAVERESTORE_H_
#define OFP_SYS_SAVERESTORE_H_

namespace ofp {
namespace sys {

OFP_BEGIN_IGNORE_PADDING

// RAII Utility class to prevent modification while iterating. (Not thread-safe)

template <class Type>
class SaveRestore {
 public:
  SaveRestore(Type &ref, Type val) : ref_{ref}, val_{ref} { ref_ = val; }

  ~SaveRestore() { ref_ = val_; }

 private:
  Type &ref_;
  Type val_;
};

OFP_END_IGNORE_PADDING

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_SAVERESTORE_H_
