// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/oxmfulltype.h"

using namespace ofp;

bool OXMFullType::parse(llvm::StringRef s) {
  // TODO(bfish): make faster
  for (size_t i = 0; i < OXMTypeInfoArraySize; ++i) {
    if (s == OXMTypeInfoArray[i].name) {
      id_ = static_cast<OXMInternalID>(i);
      type_.setValue32(OXMTypeInfoArray[i].value32);
      experimenter_ = OXMTypeInfoArray[i].experimenter;
      return true;
    }
  }

  return false;
}

std::vector<llvm::StringRef> OXMFullType::listAll() {
  std::vector<llvm::StringRef> result;
  for (size_t i = 0; i < OXMTypeInfoArraySize; ++i) {
    result.push_back(OXMTypeInfoArray[i].name);
  }
  return result;
}
