//  ===== ---- ofp/api/apievents.h -------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines API events for a api::ApiServer.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_APIEVENTS_H
#define OFP_YAML_APIEVENTS_H

#include "ofp/yaml/yllvm.h"
#include "ofp/datapathid.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/driver.h"
#include "ofp/padding.h"
#include "ofp/yaml/ybytelist.h"

OFP_BEGIN_IGNORE_PADDING

namespace ofp {  // <namespace ofp>
namespace api { // <namespace api>

/// API Event types
enum ApiEvent : UInt32 {
    LIBOFP_INVALID = 0,
    LIBOFP_LOOPBACK,                // Server -> client, client -> server
    LIBOFP_LISTEN_REQUEST,			// Client -> server
    LIBOFP_LISTEN_REPLY,            // Server -> client
    LIBOFP_YAML_ERROR,			    // Server -> client
    LIBOFP_DECODE_ERROR,            // Server -> client
    LIBOFP_DATAPATH_UP,				// Server -> client
    LIBOFP_DATAPATH_DOWN,			// Server -> client
    LIBOFP_TIMER, 					// Server -> client
    LIBOFP_SET_TIMER,				// Client -> server
    LIBOFP_EDIT_SETTING,            // Client -> server
};

/// Utility type for optional boolean values.
enum ApiBoolean {
    LIBOFP_FALSE = 0,
    LIBOFP_TRUE,
    LIBOFP_NOT_PRESENT
};

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
        UInt16 listenPort = 0;
    };
    Params params;
};

/// Api reply to ApiListenRequest.
struct ApiListenReply {
    ApiEvent event = LIBOFP_LISTEN_REPLY;

    struct Params {
        UInt16 listenPort;
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
        UInt8 version;
        UInt8 tableCount;
        UInt32 bufferCount;
        UInt32 capabilities;
        UInt32 reserved;
    };
    Params params;

    std::string toString(bool useJson);
};

/// Api message to report that a datapath connection is down.
struct ApiDatapathDown {
    ApiEvent event = LIBOFP_DATAPATH_DOWN;

    struct Params {
        DatapathID datapathId;
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

} // </namespace api>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

#define OFP_YAML_ENUMCASE(val)  io.enumCase(value, #val, ofp::api::val)

template <>
struct ScalarEnumerationTraits<ofp::api::ApiEvent> {
  static void enumeration(IO &io, ofp::api::ApiEvent &value) {
    OFP_YAML_ENUMCASE(LIBOFP_LOOPBACK);
    OFP_YAML_ENUMCASE(LIBOFP_LISTEN_REQUEST);
    OFP_YAML_ENUMCASE(LIBOFP_LISTEN_REPLY);
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
    static void mapping(IO &io, ofp::api::ApiLoopback &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiLoopback::Params> {
    static void mapping(IO &io, ofp::api::ApiLoopback::Params &msg)
    {
        io.mapOptional("validate", msg.validate, ofp::api::LIBOFP_NOT_PRESENT);
        io.mapRequired("data", msg.data);
    }
};

template <>
struct MappingTraits<ofp::api::ApiListenRequest> {
    static void mapping(IO &io, ofp::api::ApiListenRequest &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiListenRequest::Params> {
    static void mapping(IO &io, ofp::api::ApiListenRequest::Params &msg)
    {
        ofp::UInt16 defaultPort = ofp::OFP_DEFAULT_PORT;
        io.mapOptional("port", msg.listenPort, defaultPort);
    }
};

template <>
struct MappingTraits<ofp::api::ApiListenReply> {
    static void mapping(IO &io, ofp::api::ApiListenReply &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiListenReply::Params> {
    static void mapping(IO &io, ofp::api::ApiListenReply::Params &msg)
    {
        io.mapRequired("port", msg.listenPort);
        io.mapRequired("error", msg.error);
    }
};

template <>
struct MappingTraits<ofp::api::ApiYamlError> {
    static void mapping(IO &io, ofp::api::ApiYamlError &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiYamlError::Params> {
    static void mapping(IO &io, ofp::api::ApiYamlError::Params &msg)
    {
        io.mapRequired("error", msg.error);
        io.mapRequired("text", msg.text);
    }
};

template <>
struct MappingTraits<ofp::api::ApiDecodeError> {
    static void mapping(IO &io, ofp::api::ApiDecodeError &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiDecodeError::Params> {
    static void mapping(IO &io, ofp::api::ApiDecodeError::Params &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
        io.mapRequired("error", msg.error);
        io.mapRequired("data", msg.data);
    }
};

template <>
struct MappingTraits<ofp::api::ApiDatapathUp> {
    static void mapping(IO &io, ofp::api::ApiDatapathUp &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiDatapathUp::Params> {
    static void mapping(IO &io, ofp::api::ApiDatapathUp::Params &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
        io.mapRequired("version", msg.version);
        io.mapRequired("n_buffers", msg.bufferCount);
        io.mapRequired("n_tables", msg.tableCount);
        io.mapRequired("capabilities", msg.capabilities);
        io.mapRequired("reserved", msg.reserved);
    }
};


template <>
struct MappingTraits<ofp::api::ApiDatapathDown> {
    static void mapping(IO &io, ofp::api::ApiDatapathDown &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiDatapathDown::Params> {
    static void mapping(IO &io, ofp::api::ApiDatapathDown::Params &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
    }
};

template <>
struct MappingTraits<ofp::api::ApiTimer> {
    static void mapping(IO &io, ofp::api::ApiTimer &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiTimer::Params> {
    static void mapping(IO &io, ofp::api::ApiTimer::Params &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
        io.mapRequired("timer_id", msg.timerId);
    }
};

template <>
struct MappingTraits<ofp::api::ApiSetTimer> {
    static void mapping(IO &io, ofp::api::ApiSetTimer &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiSetTimer::Params> {
    static void mapping(IO &io, ofp::api::ApiSetTimer::Params &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
        io.mapRequired("timer_id", msg.timerId);
        io.mapRequired("timeout", msg.timeout);
    }
};

template <>
struct MappingTraits<ofp::api::ApiEditSetting> {
    static void mapping(IO &io, ofp::api::ApiEditSetting &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("params", msg.params);
    }
};

template <>
struct MappingTraits<ofp::api::ApiEditSetting::Params> {
    static void mapping(IO &io, ofp::api::ApiEditSetting::Params &msg)
    {
        io.mapRequired("name", msg.name);
        io.mapRequired("value", msg.value);
    }
};

} // </namespace yaml>
} // </namespace llvm>

OFP_END_IGNORE_PADDING

#endif // OFP_YAML_APIEVENTS_H
