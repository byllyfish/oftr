// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/ytablenumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s) \
  { OFPTT_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const std::pair<OFPTableNo, llvm::StringRef> sTableNumbers[] = {
    OFP_NAME(ALL),
};

const EnumConverterSparse<OFPTableNo>
    llvm::yaml::ScalarTraits<ofp::TableNumber>::converter{sTableNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
