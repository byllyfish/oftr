#include "ofp/constants.h"

using namespace ofp;

static const char *toCString(OFPType type);

std::ostream &ofp::operator<<(std::ostream &os, OFPType type) {
  return os << toCString(type);
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

  return "[unknown]";
}

#undef ENUMCASE
