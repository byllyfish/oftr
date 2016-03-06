// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/yllvm.h"

#include "ofp/yaml/ycontext.h"

using ofp::yaml::detail::YamlContext;

ofp::yaml::Encoder *ofp::yaml::GetEncoderFromContext(llvm::yaml::IO &io) {
  YamlContext *ctxt = reinterpret_cast<YamlContext *>(io.getContext());
  if (ctxt) {
    assert(ctxt->validate());
    return ctxt->encoder;
  }
  return nullptr;
}

ofp::yaml::Decoder *ofp::yaml::GetDecoderFromContext(llvm::yaml::IO &io) {
  YamlContext *ctxt = reinterpret_cast<YamlContext *>(io.getContext());
  if (ctxt) {
    assert(ctxt->validate());
    return ctxt->decoder;
  }
  return nullptr;
}

ofp::UInt8 ofp::yaml::GetVersionFromContext(llvm::yaml::IO &io) {
  YamlContext *ctxt = reinterpret_cast<YamlContext *>(io.getContext());
  if (ctxt) {
    assert(ctxt->validate());
    return ctxt->version;
  }
  return 0;
}

bool ofp::yaml::GetIncludePktMatchFromContext(llvm::yaml::IO &io) {
  YamlContext *ctxt = reinterpret_cast<YamlContext *>(io.getContext());
  if (ctxt) {
    assert(ctxt->validate());
    return ctxt->pktMatch;
  }
  return false;
}

ofp::yaml::Encoder *YamlContext::GetEncoder(void *context) {
  YamlContext *ctxt = reinterpret_cast<YamlContext *>(context);
  if (ctxt) {
    assert(ctxt->validate());
    return ctxt->encoder;
  }
  return nullptr;
}

llvm::yaml::IO *YamlContext::GetIO(void *context) {
  YamlContext *ctxt = reinterpret_cast<YamlContext *>(context);
  if (ctxt) {
    assert(ctxt->validate());
    return ctxt->io;
  }
  return nullptr;
}
