// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybuffernumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s) \
  { OFP_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static std::pair<OFPBufferNo, llvm::StringRef> sBufferIds[] = {
    OFP_NAME(NO_BUFFER)};

EnumConverterSparse<OFPBufferNo>
    llvm::yaml::ScalarTraits<ofp::BufferNumber>::converter{sBufferIds};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
