// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ycontrollermaxlen.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s) \
  { OFPCML_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static std::pair<OFPControllerMaxLen, llvm::StringRef> sControllerMaxLen[] = {
    OFP_NAME(MAX), OFP_NAME(NO_BUFFER),
};

EnumConverterSparse<OFPControllerMaxLen>
    llvm::yaml::ScalarTraits<ofp::ControllerMaxLen>::converter{
        sControllerMaxLen};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
