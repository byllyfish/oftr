// Copyright 2015-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yqueuenumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFPQ_NAME(s) \
  { OFPQ_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static std::pair<OFPQueueNo, llvm::StringRef> sQueueNumbers[] = {
    OFPQ_NAME(ALL)
};

EnumConverterSparse<OFPQueueNo>
    llvm::yaml::ScalarTraits<ofp::QueueNumber>::converter{sQueueNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
