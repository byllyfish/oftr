#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ytablenumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s) \
  { OFPTT_##s, #s }

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static std::pair<OFPTableNo, llvm::StringRef> sTableNumbers[] = {
    OFP_NAME(ALL),
};

EnumConverterSparse<OFPTableNo>
    llvm::yaml::ScalarTraits<ofp::TableNumber>::converter{sTableNumbers};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
