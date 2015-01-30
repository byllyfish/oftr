#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ytablenumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s)   { OFPTT_##s, #s }

static std::pair<OFPTableNo, llvm::StringRef> sTableNumbers[] = {
  OFP_NAME(ALL),
};

EnumConverterSparse<OFPTableNo>
    llvm::yaml::ScalarTraits<ofp::TableNumber>::converter{sTableNumbers};
