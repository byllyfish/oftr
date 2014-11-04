// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_API_APIEVENTS_H_
#define OFP_API_APIEVENTS_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/datapathid.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/driver.h"
#include "ofp/padding.h"
#include "ofp/yaml/ybytelist.h"
#include "ofp/yaml/yaddress.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp {
namespace api {

/// API Event types
enum ApiEvent : UInt32 {
  LIBOFP_INVALID = 0,
  LIBOFP_LOOPBACK,         // Server -> client, client -> server
  LIBOFP_LISTEN_REQUEST,   // Client -> server
  LIBOFP_LISTEN_REPLY,     // Server -> client
  LIBOFP_CONNECT_REQUEST,  // Client -> server
  LIBOFP_CONNECT_REPLY,    // Server -> client
  LIBOFP_YAML_ERROR,       // Server -> client
  LIBOFP_DECODE_ERROR,     // Server -> client
  LIBOFP_DATAPATH_UP,      // Server -> client
  LIBOFP_DATAPATH_DOWN,    // Server -> client
  LIBOFP_TIMER,            // Server -> client
  LIBOFP_SET_TIMER,        // Client -> server
  LIBOFP_EDIT_SETTING,     // Client -> server
};

/// Utility type for optional boolean values.
enum ApiBoolean { LIBOFP_FALSE = 0, LIBOFP_TRUE, LIBOFP_NOT_PRESENT };

/// If an OpenFlow YAML message is received before server starts listening, the
/// encoded binary data is returned by the server in a loopback message.
///
/// Client can also use loopback to translate binary to YAML. If `validate` is
/// not present, decode the hexadecimal `data` value and return the OpenFlow
/// YAML. If there is an error, return a LIBOFP_DECODE_ERROR.
///
/// If `validate` is present, check the format of the data and return an error
/// only if it doesn't meet expectations. That is, if `validate` is true, return
/// an error if the `data` is malformed. If `validate` is false, return true if
/// the `data` is _not_ detected as malformed. Nothing is returned if the
/// validation of `data` matches the `validate` boolean.
///
/// The loopback message is symmetric so it can be used in test scripts.

struct ApiLoopback {
  ApiEvent event = LIBOFP_LOOPBACK;

  struct Params {
    ByteList data;
    ApiBoolean validate;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api request to listen as a controller on a specified port.
struct ApiListenRequest {
  ApiEvent event = LIBOFP_LISTEN_REQUEST;

  struct Params {
    UInt32 xid = 0;
    IPv6Endpoint endpoint;
  };
  Params params;
};

/// Api reply to ApiListenRequest.
struct ApiListenReply {
  ApiEvent event = LIBOFP_LISTEN_REPLY;

  struct Params {
    UInt32 xid = 0;
    IPv6Endpoint endpoint;
    std::string error;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api request to connect to as a controller to the specified address and port.
struct ApiConnectRequest {
  ApiEvent event = LIBOFP_CONNECT_REQUEST;

  struct Params {
    UInt32 xid = 0;
    IPv6Endpoint endpoint;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api reply to ApiConnectRequest.
struct ApiConnectReply {
  ApiEvent event = LIBOFP_CONNECT_REPLY;

  struct Params {
    UInt32 xid = 0;
    IPv6Endpoint endpoint;
    std::string error;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api message to report an error parsing some YAML presented by the client.
struct ApiYamlError {
  ApiEvent event = LIBOFP_YAML_ERROR;

  struct Params {
    std::string error;
    std::string text;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api message to report an error decoding an OpenFlow message and translating
/// it to YAML.
struct ApiDecodeError {
  ApiEvent event = LIBOFP_DECODE_ERROR;

  struct Params {
    DatapathID datapathId;
    std::string error;
    std::string data;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api message to report that a datapath is Up.
struct ApiDatapathUp {
  ApiEvent event = LIBOFP_DATAPATH_UP;

  struct Params {
    DatapathID datapathId;
    IPv6Endpoint endpoint;
    UInt8 version;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api message to report that a datapath connection is down.
struct ApiDatapathDown {
  ApiEvent event = LIBOFP_DATAPATH_DOWN;

  struct Params {
    DatapathID datapathId;
    IPv6Endpoint endpoint;
    UInt8 version;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api message to report that a timer has expired.
struct ApiTimer {
  ApiEvent event = LIBOFP_TIMER;

  struct Params {
    DatapathID datapathId;
    UInt32 timerId;
  };
  Params params;

  std::string toString(bool useJson);
};

/// Api message to schedule a timer on a datapath channel.
struct ApiSetTimer {
  ApiEvent event = LIBOFP_SET_TIMER;

  struct Params {
    DatapathID datapathId;
    UInt32 timerId;
    UInt32 timeout;
  };
  Params params;
};

/// Api message to change the value of a protocol setting.
struct ApiEditSetting {
  ApiEvent event = LIBOFP_EDIT_SETTING;

  struct Params {
    std::string name;
    std::string value;
  };
  Params params;
};

}  // namespace api
}  // namespace ofp

namespace llvm {
namespace yaml {

#define OFP_YAML_ENUMCASE(val) io.enumCase(value, #val, ofp::api::val)

template <>
struct ScalarEnumerationTraits<ofp::api::ApiEvent> {
  static void enumeration(IO &io, ofp::api::ApiEvent &value) {
    OFP_YAML_ENUMCASE(LIBOFP_LOOPBACK);
    OFP_YAML_ENUMCASE(LIBOFP_LISTEN_REQUEST);
    OFP_YAML_ENUMCASE(LIBOFP_LISTEN_REPLY);
    OFP_YAML_ENUMCASE(LIBOFP_CONNECT_REQUEST);
    OFP_YAML_ENUMCASE(LIBOFP_CONNECT_REPLY);
    OFP_YAML_ENUMCASE(LIBOFP_YAML_ERROR);
    OFP_YAML_ENUMCASE(LIBOFP_DECODE_ERROR);
    OFP_YAML_ENUMCASE(LIBOFP_DATAPATH_UP);
    OFP_YAML_ENUMCASE(LIBOFP_DATAPATH_DOWN);
    OFP_YAML_ENUMCASE(LIBOFP_TIMER);
    OFP_YAML_ENUMCASE(LIBOFP_SET_TIMER);
    OFP_YAML_ENUMCASE(LIBOFP_EDIT_SETTING);
  }
};

#undef OFP_YAML_ENUMCASE

template <>
struct ScalarEnumerationTraits<ofp::api::ApiBoolean> {
  static void enumeration(IO &io, ofp::api::ApiBoolean &value) {
    io.enumCase(value, "true", ofp::api::LIBOFP_TRUE);
    io.enumCase(value, "false", ofp::api::LIBOFP_FALSE);
    io.enumCase(value, "not_present", ofp::api::LIBOFP_NOT_PRESENT);
  }
};

template <>
struct MappingTraits<ofp::api::ApiLoopback> {
  static void mapping(IO &io, ofp::api::ApiLoopback &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiLoopback::Params> {
  static void mapping(IO &io, ofp::api::ApiLoopback::Params &msg) {
    io.mapOptional("validate", msg.validate, ofp::api::LIBOFP_NOT_PRESENT);
    io.mapRequired("data", msg.data);
  }
};

template <>
struct MappingTraits<ofp::api::ApiListenRequest> {
  static void mapping(IO &io, ofp::api::ApiListenRequest &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiListenRequest::Params> {
  static void mapping(IO &io, ofp::api::ApiListenRequest::Params &msg) {
    io.mapRequired("xid", msg.xid);
    io.mapRequired("endpoint", msg.endpoint);
  }
};

template <>
struct MappingTraits<ofp::api::ApiListenReply> {
  static void mapping(IO &io, ofp::api::ApiListenReply &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiListenReply::Params> {
  static void mapping(IO &io, ofp::api::ApiListenReply::Params &msg) {
    io.mapRequired("xid", msg.xid);
    io.mapRequired("endpoint", msg.endpoint);
    io.mapRequired("error", msg.error);
  }
};

template <>
struct MappingTraits<ofp::api::ApiConnectRequest> {
  static void mapping(IO &io, ofp::api::ApiConnectRequest &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiConnectRequest::Params> {
  static void mapping(IO &io, ofp::api::ApiConnectRequest::Params &msg) {
    io.mapRequired("xid", msg.xid);
    io.mapRequired("endpoint", msg.endpoint);
  }
};

template <>
struct MappingTraits<ofp::api::ApiConnectReply> {
  static void mapping(IO &io, ofp::api::ApiConnectReply &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiConnectReply::Params> {
  static void mapping(IO &io, ofp::api::ApiConnectReply::Params &msg) {
    io.mapRequired("xid", msg.xid);
    io.mapRequired("endpoint", msg.endpoint);
    io.mapRequired("error", msg.error);
  }
};

template <>
struct MappingTraits<ofp::api::ApiYamlError> {
  static void mapping(IO &io, ofp::api::ApiYamlError &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiYamlError::Params> {
  static void mapping(IO &io, ofp::api::ApiYamlError::Params &msg) {
    io.mapRequired("error", msg.error);
    io.mapRequired("text", msg.text);
  }
};

template <>
struct MappingTraits<ofp::api::ApiDecodeError> {
  static void mapping(IO &io, ofp::api::ApiDecodeError &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiDecodeError::Params> {
  static void mapping(IO &io, ofp::api::ApiDecodeError::Params &msg) {
    io.mapRequired("datapath_id", msg.datapathId);
    io.mapRequired("error", msg.error);
    io.mapRequired("data", msg.data);
  }
};

template <>
struct MappingTraits<ofp::api::ApiDatapathUp> {
  static void mapping(IO &io, ofp::api::ApiDatapathUp &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiDatapathUp::Params> {
  static void mapping(IO &io, ofp::api::ApiDatapathUp::Params &msg) {
    io.mapRequired("datapath_id", msg.datapathId);
    io.mapRequired("endpoint", msg.endpoint);
    io.mapRequired("version", msg.version);
  }
};

template <>
struct MappingTraits<ofp::api::ApiDatapathDown> {
  static void mapping(IO &io, ofp::api::ApiDatapathDown &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiDatapathDown::Params> {
  static void mapping(IO &io, ofp::api::ApiDatapathDown::Params &msg) {
    io.mapRequired("datapath_id", msg.datapathId);
    io.mapRequired("endpoint", msg.endpoint);
    io.mapRequired("version", msg.version);
  }
};

template <>
struct MappingTraits<ofp::api::ApiTimer> {
  static void mapping(IO &io, ofp::api::ApiTimer &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiTimer::Params> {
  static void mapping(IO &io, ofp::api::ApiTimer::Params &msg) {
    io.mapRequired("datapath_id", msg.datapathId);
    io.mapRequired("timer_id", msg.timerId);
  }
};

template <>
struct MappingTraits<ofp::api::ApiSetTimer> {
  static void mapping(IO &io, ofp::api::ApiSetTimer &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiSetTimer::Params> {
  static void mapping(IO &io, ofp::api::ApiSetTimer::Params &msg) {
    io.mapRequired("datapath_id", msg.datapathId);
    io.mapRequired("timer_id", msg.timerId);
    io.mapRequired("timeout", msg.timeout);
  }
};

template <>
struct MappingTraits<ofp::api::ApiEditSetting> {
  static void mapping(IO &io, ofp::api::ApiEditSetting &msg) {
    io.mapRequired("event", msg.event);
    io.mapRequired("params", msg.params);
  }
};

template <>
struct MappingTraits<ofp::api::ApiEditSetting::Params> {
  static void mapping(IO &io, ofp::api::ApiEditSetting::Params &msg) {
    io.mapRequired("name", msg.name);
    io.mapRequired("value", msg.value);
  }
};

}  // namespace yaml
}  // namespace llvm

OFP_END_IGNORE_PADDING

#endif  // OFP_API_APIEVENTS_H_
