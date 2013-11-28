//  ===== ---- ofp/yaml/apievents.h ------------------------*- C++ -*- =====  //
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
/// \brief Defines API events for a yaml::ApiServer.
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
namespace yaml { // <namespace yaml>

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

/// Api message to translate binary to YAML. If `validate` is not present,
/// decode the hexadecimal `data` value and return the OpenFlow YAML. If there
/// is an error, return a LIBOFP_DECODE_ERROR.
///
/// If `validate` is present, check the format of the data and return an error
/// only if it doesn't meet expectations. That is, if `validate` is true, return
/// an error if the `data` is malformed. If `validate` is false, return true if
/// the `data` is _not_ detected as malformed. Nothing is returned if the
/// validation of `data` matches the `validate` boolean.

struct ApiLoopback {
    ApiEvent event = LIBOFP_LOOPBACK;

    struct Message {
        ByteList data;
        ApiBoolean validate;
    };
    Message msg;

    std::string toString(bool useJson);
};

/// Api request to listen as a controller on a specified port.
struct ApiListenRequest {
    ApiEvent event = LIBOFP_LISTEN_REQUEST;

    struct Message {
        UInt16 listenPort = 0;
    };
    Message msg;
};

/// Api reply to ApiListenRequest.
struct ApiListenReply {
    ApiEvent event = LIBOFP_LISTEN_REPLY;

    struct Message {
        UInt16 listenPort;
        std::string error;
    };
    Message msg;

    std::string toString(bool useJson);
};

/// Api message to report an error parsing some YAML presented by the client.
struct ApiYamlError {
    ApiEvent event = LIBOFP_YAML_ERROR;

    struct Message {
        std::string error;
        std::string text;
    };
    Message msg;

    std::string toString(bool useJson);
};

/// Api message to report an error decoding an OpenFlow message and translating
/// it to YAML.
struct ApiDecodeError {
    ApiEvent event = LIBOFP_DECODE_ERROR;

    struct Message {
        DatapathID datapathId;
        std::string error;
        std::string data;
    };
    Message msg;

    std::string toString(bool useJson);
};

/// Api message to report that a datapath is Up.
struct ApiDatapathUp {
    ApiEvent event = LIBOFP_DATAPATH_UP;

    struct Message {
        DatapathID datapathId;
        UInt8 version;
        UInt8 tableCount;
        UInt32 bufferCount;
        UInt32 capabilities;
        UInt32 reserved;
    };
    Message msg;

    std::string toString(bool useJson);
};

/// Api message to report that a datapath connection is down.
struct ApiDatapathDown {
    ApiEvent event = LIBOFP_DATAPATH_DOWN;

    struct Message {
        DatapathID datapathId;
    };
    Message msg;

    std::string toString(bool useJson);
};

/// Api message to report that a timer has expired.
struct ApiTimer {
    ApiEvent event = LIBOFP_TIMER;

    struct Message {
        DatapathID datapathId;
        UInt32 timerId;
    };
    Message msg;

    std::string toString(bool useJson);
};

/// Api message to schedule a timer on a datapath channel.
struct ApiSetTimer {
    ApiEvent event = LIBOFP_SET_TIMER;

    struct Message {
        DatapathID datapathId;
        UInt32 timerId;
        UInt32 timeout;
    };
    Message msg;
};


/// Api message to change the value of a protocol setting.
struct ApiEditSetting {
    ApiEvent event = LIBOFP_EDIT_SETTING;

    struct Message {
        std::string name;
        std::string value;
    };
    Message msg;
};

} // </namespace yaml>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>

#define OFP_YAML_ENUMCASE(val)  io.enumCase(value, #val, ofp::yaml::val)

template <>
struct ScalarEnumerationTraits<ofp::yaml::ApiEvent> {
  static void enumeration(IO &io, ofp::yaml::ApiEvent &value) {
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
struct ScalarEnumerationTraits<ofp::yaml::ApiBoolean> {
  static void enumeration(IO &io, ofp::yaml::ApiBoolean &value) {
    io.enumCase(value, "true", ofp::yaml::LIBOFP_TRUE);
    io.enumCase(value, "false", ofp::yaml::LIBOFP_FALSE);
    io.enumCase(value, "not_present", ofp::yaml::LIBOFP_NOT_PRESENT);
  }
};

template <>
struct MappingTraits<ofp::yaml::ApiLoopback> {
    static void mapping(IO &io, ofp::yaml::ApiLoopback &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiLoopback::Message> {
    static void mapping(IO &io, ofp::yaml::ApiLoopback::Message &msg)
    {
        io.mapOptional("validate", msg.validate, ofp::yaml::LIBOFP_NOT_PRESENT);
        io.mapRequired("data", msg.data);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiListenRequest> {
    static void mapping(IO &io, ofp::yaml::ApiListenRequest &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiListenRequest::Message> {
    static void mapping(IO &io, ofp::yaml::ApiListenRequest::Message &msg)
    {
        ofp::UInt16 defaultPort = ofp::OFP_DEFAULT_PORT;
        io.mapOptional("port", msg.listenPort, defaultPort);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiListenReply> {
    static void mapping(IO &io, ofp::yaml::ApiListenReply &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiListenReply::Message> {
    static void mapping(IO &io, ofp::yaml::ApiListenReply::Message &msg)
    {
        io.mapRequired("port", msg.listenPort);
        io.mapRequired("error", msg.error);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiYamlError> {
    static void mapping(IO &io, ofp::yaml::ApiYamlError &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiYamlError::Message> {
    static void mapping(IO &io, ofp::yaml::ApiYamlError::Message &msg)
    {
        io.mapRequired("error", msg.error);
        io.mapRequired("text", msg.text);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiDecodeError> {
    static void mapping(IO &io, ofp::yaml::ApiDecodeError &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiDecodeError::Message> {
    static void mapping(IO &io, ofp::yaml::ApiDecodeError::Message &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
        io.mapRequired("error", msg.error);
        io.mapRequired("data", msg.data);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiDatapathUp> {
    static void mapping(IO &io, ofp::yaml::ApiDatapathUp &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiDatapathUp::Message> {
    static void mapping(IO &io, ofp::yaml::ApiDatapathUp::Message &msg)
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
struct MappingTraits<ofp::yaml::ApiDatapathDown> {
    static void mapping(IO &io, ofp::yaml::ApiDatapathDown &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiDatapathDown::Message> {
    static void mapping(IO &io, ofp::yaml::ApiDatapathDown::Message &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiTimer> {
    static void mapping(IO &io, ofp::yaml::ApiTimer &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiTimer::Message> {
    static void mapping(IO &io, ofp::yaml::ApiTimer::Message &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
        io.mapRequired("timer_id", msg.timerId);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiSetTimer> {
    static void mapping(IO &io, ofp::yaml::ApiSetTimer &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiSetTimer::Message> {
    static void mapping(IO &io, ofp::yaml::ApiSetTimer::Message &msg)
    {
        io.mapRequired("datapath_id", msg.datapathId);
        io.mapRequired("timer_id", msg.timerId);
        io.mapRequired("timeout", msg.timeout);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiEditSetting> {
    static void mapping(IO &io, ofp::yaml::ApiEditSetting &msg)
    {
        io.mapRequired("event", msg.event);
        io.mapRequired("msg", msg.msg);
    }
};

template <>
struct MappingTraits<ofp::yaml::ApiEditSetting::Message> {
    static void mapping(IO &io, ofp::yaml::ApiEditSetting::Message &msg)
    {
        io.mapRequired("name", msg.name);
        io.mapRequired("value", msg.value);
    }
};

} // </namespace yaml>
} // </namespace llvm>

OFP_END_IGNORE_PADDING

#endif // OFP_YAML_APIEVENTS_H
