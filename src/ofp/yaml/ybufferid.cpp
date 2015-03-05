// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybufferid.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s) \
  { OFP_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static std::pair<OFPBufferID, llvm::StringRef> sBufferIds[] = {
    OFP_NAME(NO_BUFFER)};

EnumConverterSparse<OFPBufferID>
    llvm::yaml::ScalarTraits<ofp::BufferID>::converter{sBufferIds};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
