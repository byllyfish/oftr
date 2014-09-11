#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yconstants.h"

using namespace ofp;
using ofp::yaml::EnumConverter;

static const llvm::StringRef sFlowModCommands[] = {"OFPFC_ADD", "OFPFC_MODIFY",
                                         "OFPFC_MODIFY_STRICT", "OFPFC_DELETE",
                                         "OFPFC_DELETE_STRICT"};

static const llvm::StringRef sPacketInReason[] = {"OFPR_NO_MATCH", "OFPR_ACTION", "OFPR_INVALID_TTL"};

EnumConverter<OFPFlowModCommand>
    llvm::yaml::ScalarTraits<OFPFlowModCommand>::converter{sFlowModCommands};

EnumConverter<OFPPacketInReason>
    llvm::yaml::ScalarTraits<OFPPacketInReason>::converter{sPacketInReason};
