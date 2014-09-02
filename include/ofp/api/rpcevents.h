//  ===== ---- ofp/api/rpcevents.h -------------------------*- C++ -*- =====  //
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
/// \brief Implements ....
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_API_RPCEVENTS_H_
#define OFP_API_RPCEVENTS_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/datapathid.h"
#include "ofp/yaml/ydatapathid.h"
#include "ofp/driver.h"
#include "ofp/padding.h"
#include "ofp/yaml/ybytelist.h"
#include "ofp/yaml/yaddress.h"
#include "ofp/yaml/encoder.h"

namespace ofp {
namespace api {

const UInt64 RPC_ID_MISSING = 0xffffffffffffffffUL;

/// RPC Methods
enum RpcMethod : UInt32 {
  METHOD_OPEN,       // ofp.open
  METHOD_CLOSE,      // ofp.close
  METHOD_SEND,       // ofp.send
  METHOD_SET_TIMER,  // ofp.set_timer
  METHOD_CONFIG,     // ofp.config
  METHOD_DATAPATH,
  METHOD_MESSAGE,
  METHOD_TIMER,
  METHOD_UNSUPPORTED
};

enum RpcErrorCode {
  ERROR_CODE_INVALID_REQUEST = -32600
};

OFP_BEGIN_IGNORE_PADDING

/// Rpc request to open a connection.
struct RpcOpen {
  explicit RpcOpen(UInt64 ident) : id{ident} {}

  struct Params {
    /// Endpoint to connect to or listen on.
    IPv6Endpoint endpoint;
    /// Array of options passed to open.
    std::vector<std::string> options;
  };

  UInt64 id;
  Params params;
};

/// Rpc response to METHOD_OPEN.
struct RpcOpenResponse {
  explicit RpcOpenResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Result {
    /// Connection ID of opened connection.
    UInt64 connId;
    /// Status of open connection: `listening` or `connecting`.
    std::string status;
  };

  UInt64 id;
  Result result;


};

/// Rpc request to close a connection.
struct RpcClose {
  explicit RpcClose(UInt64 ident) : id{ident} {}

  struct Params {
    /// Connection ID to close.
    UInt64 connId;
  };

  UInt64 id;
  Params params;
};

/// Rpc response to METHOD_CLOSE.
struct RpcCloseResponse {
  /// Count of closed connections (0 or 1).
  UInt32 count;
};

/// Rpc request to set a timer on a specific datapath.
struct RpcSetTimer {
  explicit RpcSetTimer(UInt64 ident) : id{ident} {}

  struct Params {
    DatapathID datapathId;
    UInt32 timerId;
    UInt32 timeout;
  };

  UInt64 id;
  Params params;
};

/// Rpc notification for Message from client to be sent to datapath.
struct RpcSend {
  explicit RpcSend(UInt64 ident, yaml::Encoder::ChannelFinder finder) : id{ident}, params{finder} {}

  UInt64 id;
  yaml::Encoder params;
};

/// Rpc resposne to METHOD_SEND.
struct RpcSendResponse {
  explicit RpcSendResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Result {
    ByteRange data;
  };

  UInt64 id;
  Result result;
};

struct RpcConfig {
  explicit RpcConfig(UInt64 ident) : id{ident} {}

  struct Params {
    std::vector<std::string> options;
  };

  UInt64 id;
  Params params;
};

/// Rpc error response.
struct RpcErrorResponse {
  explicit RpcErrorResponse(UInt64 ident) : id{ident} {}
  std::string toJson();

  struct Error {
    int code;
    std::string message;
  };

  UInt64 id;
  Error error;
};

struct RpcDatapath {
  std::string toJson();

  struct Params {
    DatapathID datapathId;
    IPv6Endpoint endpoint;
    std::string status;
    UInt8 version{};
  };

  Params params;
};

OFP_END_IGNORE_PADDING

}  // namespace api
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct ScalarEnumerationTraits<ofp::api::RpcMethod> {
  static void enumeration(IO &io, ofp::api::RpcMethod &value) {
    io.enumCase(value, "ofp.open", ofp::api::METHOD_OPEN);
    io.enumCase(value, "ofp.close", ofp::api::METHOD_CLOSE);
    io.enumCase(value, "ofp.send", ofp::api::METHOD_SEND);
    io.enumCase(value, "ofp.config", ofp::api::METHOD_CONFIG);
    io.enumCase(value, "ofp.set_timer", ofp::api::METHOD_SET_TIMER);
    io.enumCase(value, "ofp.datapath", ofp::api::METHOD_DATAPATH);
    io.enumCase(value, "ofp.message", ofp::api::METHOD_MESSAGE);
    io.enumCase(value, "ofp.timer", ofp::api::METHOD_TIMER);
  }
};

template <>
struct MappingTraits<ofp::api::RpcOpen::Params> {
  static void mapping(IO &io, ofp::api::RpcOpen::Params &params) {
    io.mapRequired("endpoint", params.endpoint);
    io.mapOptional("options", params.options);
  }
};

template <>
struct MappingTraits<ofp::api::RpcClose::Params> {
  static void mapping(IO &io, ofp::api::RpcClose::Params &params) {
    io.mapRequired("conn_id", params.connId);
  }
};

template <>
struct MappingTraits<ofp::api::RpcSetTimer::Params> {
  static void mapping(IO &io, ofp::api::RpcSetTimer::Params &params) {
    io.mapRequired("datapath_id", params.datapathId);
    io.mapRequired("timer_id", params.timerId);
    io.mapRequired("timeout", params.timeout);
  }
};

template <>
struct MappingTraits<ofp::api::RpcConfig::Params> {
  static void mapping(IO &io, ofp::api::RpcConfig::Params &params) {
    io.mapRequired("options", params.options);
  }
};

template <>
struct MappingTraits<ofp::api::RpcOpenResponse> {
  static void mapping(IO &io, ofp::api::RpcOpenResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::api::RpcOpenResponse::Result> {
  static void mapping(IO &io, ofp::api::RpcOpenResponse::Result &result) {
    io.mapRequired("conn_id", result.connId);
  }
};

template <>
struct MappingTraits<ofp::api::RpcSendResponse> {
  static void mapping(IO &io, ofp::api::RpcSendResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("result", response.result);
  }
};

template <>
struct MappingTraits<ofp::api::RpcSendResponse::Result> {
  static void mapping(IO &io, ofp::api::RpcSendResponse::Result &result) {
    io.mapRequired("data", result.data);
  }
};

template <>
struct MappingTraits<ofp::api::RpcErrorResponse> {
  static void mapping(IO &io, ofp::api::RpcErrorResponse &response) {
    io.mapRequired("id", response.id);
    io.mapRequired("error", response.error);
  }
};

template <>
struct MappingTraits<ofp::api::RpcErrorResponse::Error> {
  static void mapping(IO &io, ofp::api::RpcErrorResponse::Error &error) {
    io.mapRequired("code", error.code);
    io.mapRequired("message", error.message);
  }
};

template <>
struct MappingTraits<ofp::api::RpcDatapath> {
  static void mapping(IO &io, ofp::api::RpcDatapath &response) {
    ofp::api::RpcMethod method = ofp::api::METHOD_DATAPATH;
    io.mapRequired("method", method);
    io.mapRequired("params", response.params);
  }
};

template <>
struct MappingTraits<ofp::api::RpcDatapath::Params> {
  static void mapping(IO &io, ofp::api::RpcDatapath::Params &params) {
    io.mapRequired("datapath_id", params.datapathId);
    io.mapRequired("version", params.version);
    io.mapRequired("endpoint", params.endpoint);
    io.mapRequired("status", params.status);
  }
};


}  // namespace yaml
}  // namespace llvm

#endif  // OFP_API_RPCEVENTS_H_
