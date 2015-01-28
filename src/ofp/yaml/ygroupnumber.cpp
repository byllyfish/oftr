#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ygroupnumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s)   { OFPG_##s, #s }

static std::pair<OFPGroupNo, llvm::StringRef> sGroupNumbers[] = {
  OFP_NAME(ALL),
  OFP_NAME(ANY),
};

EnumConverterSparse<OFPGroupNo>
    llvm::yaml::ScalarTraits<ofp::GroupNumber>::converter{sGroupNumbers};
