// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/ymeternumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFPM_NAME(s) \
  { OFPM_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const std::pair<OFPMeterNo, llvm::StringRef> sMeterNumbers[] = {
    OFPM_NAME(SLOWPATH), OFPM_NAME(CONTROLLER), OFPM_NAME(ALL)};

const EnumConverterSparse<OFPMeterNo>
    llvm::yaml::ScalarTraits<ofp::MeterNumber>::converter{sMeterNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
