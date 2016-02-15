// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/yaml/yllvm.h"
#include "ofp/yaml/yconstants.h"

using namespace ofp;
using ofp::yaml::EnumConverter;
using ofp::yaml::EnumConverterSparse;

OFP_BEGIN_IGNORE_GLOBAL_CONSTRUCTOR

static const llvm::StringRef sTypes[] = {
    "HELLO", "ERROR", "ECHO_REQUEST", "ECHO_REPLY", "EXPERIMENTER",
    "FEATURES_REQUEST", "FEATURES_REPLY", "GET_CONFIG_REQUEST",
    "GET_CONFIG_REPLY", "SET_CONFIG", "PACKET_IN", "FLOW_REMOVED",
    "PORT_STATUS", "PACKET_OUT", "FLOW_MOD", "GROUP_MOD", "PORT_MOD",
    "TABLE_MOD", "MULTIPART_REQUEST", "MULTIPART_REPLY", "BARRIER_REQUEST",
    "BARRIER_REPLY", "QUEUE_GET_CONFIG_REQUEST", "QUEUE_GET_CONFIG_REPLY",
    "ROLE_REQUEST", "ROLE_REPLY", "GET_ASYNC_REQUEST", "GET_ASYNC_REPLY",
    "SET_ASYNC", "METER_MOD", "ROLE_STATUS", "TABLE_STATUS", "REQUESTFORWARD",
    "BUNDLE_CONTROL", "BUNDLE_ADD_MESSAGE"};

static const llvm::StringRef sMultipartTypes[] = {
    "DESC",           "FLOW",      "AGGREGATE",    "TABLE",
    "PORT_STATS",     "QUEUE",     "GROUP",        "GROUP_DESC",
    "GROUP_FEATURES", "METER",     "METER_CONFIG", "METER_FEATURES",
    "TABLE_FEATURES", "PORT_DESC", "TABLE_DESC",   "QUEUE_DESC",
    "FLOW_MONITOR",
};

static const llvm::StringRef sInstructionTypes[] = {
    "NONE",          "GOTO_TABLE",    "WRITE_METADATA", "WRITE_ACTIONS",
    "APPLY_ACTIONS", "CLEAR_ACTIONS", "METER",
};

static const llvm::StringRef sMeterBandTypes[] = {
    "NONE", "DROP", "DSCP_REMARK",
};

static const llvm::StringRef sFlowModCommands[] = {
    "ADD", "MODIFY", "MODIFY_STRICT", "DELETE", "DELETE_STRICT"};

static const llvm::StringRef sPacketInReason[] = {
    "TABLE_MISS", "APPLY_ACTION", "INVALID_TTL",
    "ACTION_SET", "GROUP",        "PACKET_OUT",
};

static const llvm::StringRef sPortStatusReason[] = {
    "ADD", "DELETE", "MODIFY",
};

static const llvm::StringRef sFlowRemovedReason[] = {
    "IDLE_TIMEOUT", "HARD_TIMEOUT", "DELETE",
    "GROUP_DELETE", "METER_DELETE", "EVICTION"};

static const llvm::StringRef sTableStatusReason[] = {
    "0x00", "0x01", "VACANCY_DOWN", "VACANCY_UP"};

static const llvm::StringRef sControllerRole[] = {
    "ROLE_NOCHANGE", "ROLE_EQUAL", "ROLE_MASTER", "ROLE_SLAVE",
};

static const llvm::StringRef sMeterModCommand[] = {
    "ADD", "MODIFY", "DELETE",
};

static const llvm::StringRef sGroupModCommand[] = {
    "ADD", "MODIFY", "DELETE",
};

static const llvm::StringRef sGroupType[] = {
    "ALL", "SELECT", "INDIRECT", "FF",
};

static const llvm::StringRef sErrorTypes[] = {
    "HELLO_FAILED", "BAD_REQUEST", "BAD_ACTION", "BAD_INSTRUCTION", "BAD_MATCH",
    "FLOW_MOD_FAILED", "GROUP_MOD_FAILED", "PORT_MOD_FAILED",
    "TABLE_MOD_FAILED", "QUEUE_OP_FAILED", "SWITCH_CONFIG_FAILED",
    "ROLE_REQUEST_FAILED", "METER_MOD_FAILED", "TABLE_FEATURES_FAILED",
    "BAD_PROPERTY", "ASYNC_CONFIG_FAILED", "FLOW_MONITOR_FAILED",
    "BUNDLE_FAILED"};

static const llvm::StringRef sFlowUpdateEvent[] = {
    "INITIAL", "ADDED", "REMOVED", "MODIFIED", "ABBREV", "PAUSED", "RESUMED"};

static const llvm::StringRef sFlowMonitorCommand[] = {"ADD", "MODIFY",
                                                      "DELETE"};

static const llvm::StringRef sBundleCtrlType[] = {
    "OPEN_REQUEST",   "OPEN_REPLY",   "CLOSE_REQUEST",   "CLOSE_REPLY",
    "COMMIT_REQUEST", "COMMIT_REPLY", "DISCARD_REQUEST", "DISCARD_REPLY"};

static const llvm::StringRef sExperimenterPropertyList[] = {"EXPERIMENTER_0"};

static const llvm::StringRef sRoleStatusReason[] = {
    "MASTER_REQUEST", "CONFIG", "EXPERIMENTER",
};

const EnumConverter<OFPType> llvm::yaml::ScalarTraits<OFPType>::converter{sTypes};

const EnumConverter<OFPMultipartType>
    llvm::yaml::ScalarTraits<OFPMultipartType>::converter{sMultipartTypes,
                                                          "EXPERIMENTER"};

const EnumConverter<OFPInstructionType>
    llvm::yaml::ScalarTraits<OFPInstructionType>::converter{sInstructionTypes,
                                                            "EXPERIMENTER"};

const EnumConverter<OFPMeterBandType>
    llvm::yaml::ScalarTraits<OFPMeterBandType>::converter{sMeterBandTypes,
                                                          "EXPERIMENTER"};

const EnumConverter<OFPFlowModCommand>
    llvm::yaml::ScalarTraits<OFPFlowModCommand>::converter{sFlowModCommands};

const EnumConverter<OFPPacketInReason>
    llvm::yaml::ScalarTraits<OFPPacketInReason>::converter{sPacketInReason};

const EnumConverter<OFPPortStatusReason>
    llvm::yaml::ScalarTraits<OFPPortStatusReason>::converter{sPortStatusReason};

const EnumConverter<OFPFlowRemovedReason>
    llvm::yaml::ScalarTraits<OFPFlowRemovedReason>::converter{
        sFlowRemovedReason};

const EnumConverter<OFPTableStatusReason>
    llvm::yaml::ScalarTraits<OFPTableStatusReason>::converter{
        sTableStatusReason};

const EnumConverter<OFPControllerRole>
    llvm::yaml::ScalarTraits<OFPControllerRole>::converter{sControllerRole};

const EnumConverter<OFPMeterModCommand>
    llvm::yaml::ScalarTraits<OFPMeterModCommand>::converter{sMeterModCommand};

const EnumConverter<OFPGroupModCommand>
    llvm::yaml::ScalarTraits<OFPGroupModCommand>::converter{sGroupModCommand};

const EnumConverter<OFPGroupType> llvm::yaml::ScalarTraits<OFPGroupType>::converter{
    sGroupType};

const EnumConverter<OFPErrorType> llvm::yaml::ScalarTraits<OFPErrorType>::converter{
    sErrorTypes, "EXPERIMENTER"};

const EnumConverter<OFPFlowUpdateEvent>
    llvm::yaml::ScalarTraits<OFPFlowUpdateEvent>::converter{sFlowUpdateEvent};

const EnumConverter<OFPFlowMonitorCommand>
    llvm::yaml::ScalarTraits<OFPFlowMonitorCommand>::converter{
        sFlowMonitorCommand};

const EnumConverter<OFPBundleCtrlType>
    llvm::yaml::ScalarTraits<OFPBundleCtrlType>::converter{sBundleCtrlType};

const EnumConverter<OFPExperimenterPropertyType>
    llvm::yaml::ScalarTraits<OFPExperimenterPropertyType>::converter{
        sExperimenterPropertyList, "EXPERIMENTER"};

const EnumConverter<OFPRoleStatusReason>
    llvm::yaml::ScalarTraits<OFPRoleStatusReason>::converter{sRoleStatusReason};

#define STR_ENTRY(s) \
  { s, #s }

static std::pair<OFPErrorCode, llvm::StringRef> sErrorCodes[] = {
    STR_ENTRY(OFPHFC_INCOMPATIBLE), STR_ENTRY(OFPHFC_EPERM),

    STR_ENTRY(OFPBRC_BAD_VERSION), STR_ENTRY(OFPBRC_BAD_TYPE),
    STR_ENTRY(OFPBRC_BAD_MULTIPART), STR_ENTRY(OFPBRC_BAD_EXPERIMENTER),
    STR_ENTRY(OFPBRC_BAD_EXP_TYPE), STR_ENTRY(OFPBRC_EPERM),
    STR_ENTRY(OFPBRC_BAD_LEN), STR_ENTRY(OFPBRC_BUFFER_EMPTY),
    STR_ENTRY(OFPBRC_BUFFER_UNKNOWN), STR_ENTRY(OFPBRC_BAD_TABLE_ID),
    STR_ENTRY(OFPBRC_IS_SLAVE), STR_ENTRY(OFPBRC_BAD_PORT),
    STR_ENTRY(OFPBRC_BAD_PACKET), STR_ENTRY(OFPBRC_MULTIPART_BUFFER_OVERFLOW),

    STR_ENTRY(OFPBAC_BAD_TYPE), STR_ENTRY(OFPBAC_BAD_LEN),
    STR_ENTRY(OFPBAC_BAD_EXPERIMENTER), STR_ENTRY(OFPBAC_BAD_EXP_TYPE),
    STR_ENTRY(OFPBAC_BAD_OUT_PORT), STR_ENTRY(OFPBAC_BAD_ARGUMENT),
    STR_ENTRY(OFPBAC_EPERM), STR_ENTRY(OFPBAC_TOO_MANY),
    STR_ENTRY(OFPBAC_BAD_QUEUE), STR_ENTRY(OFPBAC_BAD_OUT_GROUP),
    STR_ENTRY(OFPBAC_MATCH_INCONSISTENT), STR_ENTRY(OFPBAC_UNSUPPORTED_ORDER),
    STR_ENTRY(OFPBAC_BAD_TAG), STR_ENTRY(OFPBAC_BAD_SET_TYPE),
    STR_ENTRY(OFPBAC_BAD_SET_LEN), STR_ENTRY(OFPBAC_BAD_SET_ARGUMENT),

    STR_ENTRY(OFPBIC_UNKNOWN_INST), STR_ENTRY(OFPBIC_UNSUP_INST),
    STR_ENTRY(OFPBIC_BAD_TABLE_ID), STR_ENTRY(OFPBIC_UNSUP_METADATA),
    STR_ENTRY(OFPBIC_UNSUP_METADATA_MASK), STR_ENTRY(OFPBIC_BAD_EXPERIMENTER),
    STR_ENTRY(OFPBIC_BAD_EXP_TYPE), STR_ENTRY(OFPBIC_BAD_LEN),
    STR_ENTRY(OFPBIC_EPERM),

    STR_ENTRY(OFPBMC_BAD_TYPE), STR_ENTRY(OFPBMC_BAD_LEN),
    STR_ENTRY(OFPBMC_BAD_TAG), STR_ENTRY(OFPBMC_BAD_DL_ADDR_MASK),
    STR_ENTRY(OFPBMC_BAD_NW_ADDR_MASK), STR_ENTRY(OFPBMC_BAD_WILDCARDS),
    STR_ENTRY(OFPBMC_BAD_FIELD), STR_ENTRY(OFPBMC_BAD_VALUE),
    STR_ENTRY(OFPBMC_BAD_MASK), STR_ENTRY(OFPBMC_BAD_PREREQ),
    STR_ENTRY(OFPBMC_DUP_FIELD), STR_ENTRY(OFPBMC_EPERM),

    STR_ENTRY(OFPFMFC_UNKNOWN), STR_ENTRY(OFPFMFC_TABLE_FULL),
    STR_ENTRY(OFPFMFC_BAD_TABLE_ID), STR_ENTRY(OFPFMFC_OVERLAP),
    STR_ENTRY(OFPFMFC_EPERM), STR_ENTRY(OFPFMFC_BAD_TIMEOUT),
    STR_ENTRY(OFPFMFC_BAD_COMMAND), STR_ENTRY(OFPFMFC_BAD_FLAGS),
    STR_ENTRY(OFPFMFC_UNSUPPORTED),

    STR_ENTRY(OFPGMFC_GROUP_EXISTS), STR_ENTRY(OFPGMFC_INVALID_GROUP),
    STR_ENTRY(OFPGMFC_WEIGHT_UNSUPPORTED), STR_ENTRY(OFPGMFC_OUT_OF_GROUPS),
    STR_ENTRY(OFPGMFC_OUT_OF_BUCKETS), STR_ENTRY(OFPGMFC_CHAINING_UNSUPPORTED),
    STR_ENTRY(OFPGMFC_WATCH_UNSUPPORTED), STR_ENTRY(OFPGMFC_LOOP),
    STR_ENTRY(OFPGMFC_UNKNOWN_GROUP), STR_ENTRY(OFPGMFC_CHAINED_GROUP),
    STR_ENTRY(OFPGMFC_BAD_TYPE), STR_ENTRY(OFPGMFC_BAD_COMMAND),
    STR_ENTRY(OFPGMFC_BAD_BUCKET), STR_ENTRY(OFPGMFC_BAD_WATCH),
    STR_ENTRY(OFPGMFC_EPERM),

    STR_ENTRY(OFPPMFC_BAD_PORT), STR_ENTRY(OFPPMFC_BAD_HW_ADDR),
    STR_ENTRY(OFPPMFC_BAD_CONFIG), STR_ENTRY(OFPPMFC_BAD_ADVERTISE),
    STR_ENTRY(OFPPMFC_EPERM),

    STR_ENTRY(OFPTMFC_BAD_TABLE), STR_ENTRY(OFPTMFC_BAD_CONFIG),
    STR_ENTRY(OFPTMFC_EPERM),

    STR_ENTRY(OFPQOFC_BAD_PORT), STR_ENTRY(OFPQOFC_BAD_QUEUE),
    STR_ENTRY(OFPQOFC_EPERM),

    STR_ENTRY(OFPSCFC_BAD_FLAGS), STR_ENTRY(OFPSCFC_BAD_LEN),
    STR_ENTRY(OFPSCFC_EPERM),

    STR_ENTRY(OFPRRFC_STALE), STR_ENTRY(OFPRRFC_UNSUP),
    STR_ENTRY(OFPRRFC_BAD_ROLE),

    STR_ENTRY(OFPMMFC_UNKNOWN), STR_ENTRY(OFPMMFC_METER_EXISTS),
    STR_ENTRY(OFPMMFC_INVALID_METER), STR_ENTRY(OFPMMFC_UNKNOWN_METER),
    STR_ENTRY(OFPMMFC_BAD_COMMAND), STR_ENTRY(OFPMMFC_BAD_FLAGS),
    STR_ENTRY(OFPMMFC_BAD_RATE), STR_ENTRY(OFPMMFC_BAD_BURST),
    STR_ENTRY(OFPMMFC_BAD_BAND), STR_ENTRY(OFPMMFC_BAD_BAND_VALUE),
    STR_ENTRY(OFPMMFC_OUT_OF_METERS), STR_ENTRY(OFPMMFC_OUT_OF_BANDS),

    STR_ENTRY(OFPTFFC_BAD_TABLE), STR_ENTRY(OFPTFFC_BAD_METADATA),
    STR_ENTRY(OFPTFFC_BAD_TYPE), STR_ENTRY(OFPTFFC_BAD_LEN),
    STR_ENTRY(OFPTFFC_BAD_ARGUMENT), STR_ENTRY(OFPTFFC_EPERM),
};

static std::pair<OFPAsyncConfigProperty, llvm::StringRef>
    sAsyncConfigProperty[] = {
        {OFPACPT_EXPERIMENTER_SLAVE, "EXPERIMENTER_SLAVE"},
        {OFPACPT_EXPERIMENTER_MASTER, "EXPERIMENTER_MASTER"}};

const EnumConverterSparse<OFPErrorCode>
    llvm::yaml::ScalarTraits<OFPErrorCode>::converter{sErrorCodes};

const EnumConverterSparse<OFPAsyncConfigProperty>
    llvm::yaml::ScalarTraits<OFPAsyncConfigProperty>::converter{
        sAsyncConfigProperty};

OFP_END_IGNORE_GLOBAL_CONSTRUCTOR
