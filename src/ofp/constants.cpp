#include "ofp/constants.h"

using namespace ofp;

static const char *toCString(OFPType type);
static const char *toCString(OFPMultipartType type);

std::ostream &ofp::operator<<(std::ostream &os, OFPType type) {
  const char *s = toCString(type);
  if (s) {
    return os << s;
  }
  return os << '[' << static_cast<int>(type) << ']';
}

std::ostream &ofp::operator<<(std::ostream &os, OFPMultipartType type) {
  const char *s = toCString(type);
  if (s) {
    return os << s;
  }
  return os << '[' << static_cast<int>(type) << ']';
}

#define ENUMCASE(val)                                                          \
  case val:                                                                    \
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
    ENUMCASE(OFPT_UNSUPPORTED);
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
    ENUMCASE(OFPMP_EXPERIMENTER);
  }

  return nullptr;
}

#undef ENUMCASE
