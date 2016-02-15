// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybuffernumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s) \
  { OFP_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const std::pair<OFPBufferNo, llvm::StringRef> sBufferIds[] = {
    OFP_NAME(NO_BUFFER)};

const EnumConverterSparse<OFPBufferNo>
    llvm::yaml::ScalarTraits<ofp::BufferNumber>::converter{sBufferIds};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
