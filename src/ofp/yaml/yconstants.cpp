// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yconstants.h"

using namespace ofp;
using ofp::yaml::EnumConverter;

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const llvm::StringRef sFlowModCommands[] = {
    "OFPFC_ADD", "OFPFC_MODIFY", "OFPFC_MODIFY_STRICT", "OFPFC_DELETE",
    "OFPFC_DELETE_STRICT"};

static const llvm::StringRef sPacketInReason[] = {
    "OFPR_NO_MATCH", "OFPR_ACTION", "OFPR_INVALID_TTL"};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR

EnumConverter<OFPFlowModCommand>
    llvm::yaml::ScalarTraits<OFPFlowModCommand>::converter{sFlowModCommands};

EnumConverter<OFPPacketInReason>
    llvm::yaml::ScalarTraits<OFPPacketInReason>::converter{sPacketInReason};
