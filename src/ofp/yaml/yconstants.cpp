#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yconstants.h"

using namespace ofp;
using ofp::yaml::EnumConverter;

static const char *sFlowModCommands[] = {"OFPFC_ADD", "OFPFC_MODIFY",
                                         "OFPFC_MODIFY_STRICT", "OFPFC_DELETE",
                                         "OFPFC_DELETE_STRICT"};

EnumConverter<OFPFlowModCommand, OFPFC_LAST + 1>
    llvm::yaml::ScalarTraits<OFPFlowModCommand>::converter{sFlowModCommands};
