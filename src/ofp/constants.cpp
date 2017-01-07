// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/constants.h"

using namespace ofp;

UInt16 ofp::OFPGetDefaultPort() {
  const char *portStr = std::getenv("LIBOFP_DEFAULT_PORT");
  if (!portStr)
    return OFP_DEFAULT_PORT;

  int portNum = static_cast<int>(std::strtol(portStr, nullptr, 10));
  if (portNum > 0 && portNum < 0xffff) {
    return UInt16_narrow_cast(portNum);
  }

  return OFP_DEFAULT_PORT;
}

static const char *toCString(OFPType type);
static const char *toCString(OFPMultipartType type);

llvm::raw_ostream &ofp::operator<<(llvm::raw_ostream &os, OFPType type) {
  const char *s = toCString(type);
  if (s) {
    return os << s;
  }
  return os << '[' << static_cast<int>(type) << ']';
}

llvm::raw_ostream &ofp::operator<<(llvm::raw_ostream &os, OFPMultipartType type) {
  const char *s = toCString(type);
  if (s) {
    return os << s;
  }
  return os << '[' << static_cast<int>(type) << ']';
}

#define ENUMCASE(val) \
  case val:           \
    return #val

const char *toCString(OFPType type) {
  switch (type) {
    ENUMCASE(OFPT_HELLO);
    ENUMCASE(OFPT_ERROR);
    ENUMCASE(OFPT_ECHO_REQUEST);
    ENUMCASE(OFPT_ECHO_REPLY);
    ENUMCASE(OFPT_EXPERIMENTER);
    ENUMCASE(OFPT_FEATURES_REQUEST);
    ENUMCASE(OFPT_FEATURES_REPLY);
    ENUMCASE(OFPT_GET_CONFIG_REQUEST);
    ENUMCASE(OFPT_GET_CONFIG_REPLY);
    ENUMCASE(OFPT_SET_CONFIG);
    ENUMCASE(OFPT_PACKET_IN);
    ENUMCASE(OFPT_FLOW_REMOVED);
    ENUMCASE(OFPT_PORT_STATUS);
    ENUMCASE(OFPT_PACKET_OUT);
    ENUMCASE(OFPT_FLOW_MOD);
    ENUMCASE(OFPT_GROUP_MOD);
    ENUMCASE(OFPT_PORT_MOD);
    ENUMCASE(OFPT_TABLE_MOD);
    ENUMCASE(OFPT_MULTIPART_REQUEST);
    ENUMCASE(OFPT_MULTIPART_REPLY);
    ENUMCASE(OFPT_BARRIER_REQUEST);
    ENUMCASE(OFPT_BARRIER_REPLY);
    ENUMCASE(OFPT_QUEUE_GET_CONFIG_REQUEST);
    ENUMCASE(OFPT_QUEUE_GET_CONFIG_REPLY);
    ENUMCASE(OFPT_ROLE_REQUEST);
    ENUMCASE(OFPT_ROLE_REPLY);
    ENUMCASE(OFPT_GET_ASYNC_REQUEST);
    ENUMCASE(OFPT_GET_ASYNC_REPLY);
    ENUMCASE(OFPT_SET_ASYNC);
    ENUMCASE(OFPT_METER_MOD);
    ENUMCASE(OFPT_ROLE_STATUS);
    ENUMCASE(OFPT_TABLE_STATUS);
    ENUMCASE(OFPT_BUNDLE_CONTROL);
    ENUMCASE(OFPT_BUNDLE_ADD_MESSAGE);
    ENUMCASE(OFPT_REQUESTFORWARD);
    ENUMCASE(OFPT_MAX_ALLOWED);
    ENUMCASE(OFPT_UNSUPPORTED);
    ENUMCASE(OFPT_RAW_MESSAGE);
  }

  return nullptr;
}

const char *toCString(OFPMultipartType type) {
  switch (type) {
    ENUMCASE(OFPMP_DESC);
    ENUMCASE(OFPMP_FLOW);
    ENUMCASE(OFPMP_AGGREGATE);
    ENUMCASE(OFPMP_TABLE);
    ENUMCASE(OFPMP_PORT_STATS);
    ENUMCASE(OFPMP_QUEUE);
    ENUMCASE(OFPMP_GROUP);
    ENUMCASE(OFPMP_GROUP_DESC);
    ENUMCASE(OFPMP_GROUP_FEATURES);
    ENUMCASE(OFPMP_METER);
    ENUMCASE(OFPMP_METER_CONFIG);
    ENUMCASE(OFPMP_METER_FEATURES);
    ENUMCASE(OFPMP_TABLE_FEATURES);
    ENUMCASE(OFPMP_PORT_DESC);
    ENUMCASE(OFPMP_TABLE_DESC);
    ENUMCASE(OFPMP_QUEUE_DESC);
    ENUMCASE(OFPMP_FLOW_MONITOR);
    ENUMCASE(OFPMP_EXPERIMENTER);
    ENUMCASE(OFPMP_UNSUPPORTED);
  }

  return nullptr;
}

#undef ENUMCASE

OFPActionTypeFlags ofp::OFPActionTypeFlagsConvertToV1(UInt32 actions) {
  UInt32 result = actions & 0x0607;
  UInt32 moved = (actions & 0x00F8) << 1;
  result |= moved;
  if (actions & OFPATF_STRIP_VLAN_V1)
    result |= 1 << deprecated::v1::OFPAT_STRIP_VLAN;
  if (actions & OFPATF_ENQUEUE_V1)
    result |= 1 << deprecated::v1::OFPAT_ENQUEUE;
  return static_cast<OFPActionTypeFlags>(result);
}

OFPActionTypeFlags ofp::OFPActionTypeFlagsConvertFromV1(UInt32 actions) {
  UInt32 result = actions & 0x0607;
  UInt32 moved = (actions & 0x01F0) >> 1;
  result |= moved;
  if (actions & (1 << deprecated::v1::OFPAT_STRIP_VLAN))
    result |= OFPATF_STRIP_VLAN_V1;
  if (actions & (1 << deprecated::v1::OFPAT_ENQUEUE))
    result |= OFPATF_ENQUEUE_V1;
  return static_cast<OFPActionTypeFlags>(result);
}

OFPCapabilitiesFlags ofp::OFPCapabilitiesFlagsConvertToV1(UInt32 capabilities) {
  UInt32 result = capabilities & 0x7FFFFFFF;
  if (capabilities & OFPC_STP) {
    result |= deprecated::v1::OFPC_STP;
  }
  return static_cast<OFPCapabilitiesFlags>(result);
}

OFPCapabilitiesFlags ofp::OFPCapabilitiesFlagsConvertFromV1(
    UInt32 capabilities) {
  UInt32 result = capabilities;
  if (capabilities & deprecated::v1::OFPC_STP) {
    result |= OFPC_STP;
    result &= ~deprecated::v1::OFPC_STP;
  }
  return static_cast<OFPCapabilitiesFlags>(result);
}

OFPPortFeaturesFlags ofp::OFPPortFeaturesFlagsConvertToV1(UInt32 features) {
  UInt32 result = features & ~0x0FF80U;
  result |= (features & 0xF800) >> 4;
  return static_cast<OFPPortFeaturesFlags>(result);
}

OFPPortFeaturesFlags ofp::OFPPortFeaturesFlagsConvertFromV1(UInt32 features) {
  UInt32 result = features & ~0x0FF80U;
  result |= (features & 0x00F80) << 4;
  return static_cast<OFPPortFeaturesFlags>(result);
}
