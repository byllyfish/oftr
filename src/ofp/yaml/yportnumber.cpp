// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/yllvm.h"

#include "ofp/yaml/yportnumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFPP_NAME(s) \
  { OFPP_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const std::pair<OFPPortNo, llvm::StringRef> sPortNumbers[] = {
    OFPP_NAME(IN_PORT), OFPP_NAME(TABLE), OFPP_NAME(NORMAL),
    OFPP_NAME(FLOOD),   OFPP_NAME(ALL),   OFPP_NAME(CONTROLLER),
    OFPP_NAME(LOCAL),   OFPP_NAME(ANY),   OFPP_NAME(NONE),
};

const EnumConverterSparse<OFPPortNo>
    llvm::yaml::ScalarTraits<ofp::PortNumber>::converter{sPortNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
