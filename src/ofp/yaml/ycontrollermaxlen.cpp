#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ycontrollermaxlen.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s)   { OFPCML_##s, #s }

static std::pair<OFPControllerMaxLen, llvm::StringRef> sControllerMaxLen[] = {
  OFP_NAME(NO_BUFFER),
};

EnumConverterSparse<OFPControllerMaxLen>
    llvm::yaml::ScalarTraits<ofp::ControllerMaxLen>::converter{sControllerMaxLen};
