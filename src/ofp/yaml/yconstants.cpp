#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yconstants.h"

static const char *sFlowModCommands[] = {"OFPFC_ADD",           "OFPFC_MODIFY",
                                         "OFPFC_MODIFY_STRICT", "OFPFC_DELETE",
                                         "OFPFC_DELETE_STRICT"};

ofp::yaml::EnumConverter<ofp::OFPFlowModCommand, ofp::OFPFC_LAST+1> llvm::yaml::ScalarTraits<
    ofp::OFPFlowModCommand>::converter{sFlowModCommands};
