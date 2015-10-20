// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ymeternumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFPM_NAME(s) \
  { OFPM_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static std::pair<OFPMeterNo, llvm::StringRef> sMeterNumbers[] = {
    OFPM_NAME(SLOWPATH), OFPM_NAME(CONTROLLER), OFPM_NAME(ALL)};

EnumConverterSparse<OFPMeterNo>
    llvm::yaml::ScalarTraits<ofp::MeterNumber>::converter{sMeterNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
