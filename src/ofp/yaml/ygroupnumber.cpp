// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ygroupnumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s) \
  { OFPG_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static std::pair<OFPGroupNo, llvm::StringRef> sGroupNumbers[] = {
    OFP_NAME(ALL), OFP_NAME(ANY),
};

EnumConverterSparse<OFPGroupNo>
    llvm::yaml::ScalarTraits<ofp::GroupNumber>::converter{sGroupNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
