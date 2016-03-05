// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/yqueuenumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFPQ_NAME(s) \
  { OFPQ_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const std::pair<OFPQueueNo, llvm::StringRef> sQueueNumbers[] = {
    OFPQ_NAME(ALL)};

const EnumConverterSparse<OFPQueueNo>
    llvm::yaml::ScalarTraits<ofp::QueueNumber>::converter{sQueueNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
