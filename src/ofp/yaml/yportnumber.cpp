#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yportnumber.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFPP_NAME(s)   { OFPP_##s, #s }

static std::pair<OFPPortNo, llvm::StringRef> sPortNumbers[] = {
  OFPP_NAME(IN_PORT),
  OFPP_NAME(TABLE),
  OFPP_NAME(NORMAL),
  OFPP_NAME(FLOOD),
  OFPP_NAME(ALL),
  OFPP_NAME(CONTROLLER),
  OFPP_NAME(LOCAL),
  OFPP_NAME(ANY),
  OFPP_NAME(NONE),
};

EnumConverterSparse<OFPPortNo>
    llvm::yaml::ScalarTraits<ofp::PortNumber>::converter{sPortNumbers};
