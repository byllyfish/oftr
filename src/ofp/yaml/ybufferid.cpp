#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/ybufferid.h"

using namespace ofp;
using ofp::yaml::EnumConverterSparse;

#define OFP_NAME(s)   { OFP_##s, #s }

static std::pair<OFPBufferID, llvm::StringRef> sBufferIds[] = {
  OFP_NAME(NO_BUFFER)
};

EnumConverterSparse<OFPBufferID>
    llvm::yaml::ScalarTraits<ofp::BufferID>::converter{sBufferIds};
